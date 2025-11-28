#include <lartc/api/lpp.hh>
#include <lartc/api/utils.hh>
#include <lartc/api/config.hh>

#include <lartc/ast/declaration.hh>
#include <lartc/ast/declaration/parse.hh>
#include <lartc/ast/check.hh>
#include <lartc/resolve/resolve_symbols.hh>
#include <lartc/tree_sitter.hh>
#include <lartc/terminal.hh>
#include <lartc/ast/file_db.hh>
#include <lartc/typecheck/type_cache.hh>
#include <lartc/typecheck/check_types.hh>
#include <lartc/typecheck/size_cache.hh>
#include <lartc/typecheck/check_declared_types.hh>
#include <lartc/constants/check_constants.hh>
#include <lartc/constants/constant_cache.hh>
#include <lartc/codegen/cg_context.hh>
#include <lartc/codegen/emit_llvm.hh>

#include <iostream>
#include <assert.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <filesystem>

extern "C" const TSLanguage *tree_sitter_lart(void);

template<typename T, typename ... ExtraArgs>
void print_to_file(T& object, const char* filepath, ExtraArgs ... extra_args) {
  std::ofstream out (filepath);
  T::Print(out, extra_args ..., object);
  out.close();
}

template<typename T, typename ... ExtraArgs>
void print_to_file(T* object, const char* filepath, ExtraArgs ... extra_args) {
  std::ofstream out (filepath);
  T::Print(out, extra_args ..., object);
  out.close();
}

bool parse_filepath(Declaration* decl_tree, TSParser* parser, TSContext& context) {
  FileDB::File* file = context.file_db->add_file(context.filepath);
  context.source_code = file->source_code;

  TSTree *tree = ts_parser_parse_string(parser, NULL, context.source_code, strlen(context.source_code));
  TSNode root_node = ts_tree_root_node(tree);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking ts_tree for errors ... \n");
  }
  bool ast_ok = check_ts_tree_for_errors(context, root_node);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking ts_tree for errors ... OK\n");
  }

  if (ast_ok) {
    context.ok = true;
    if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
      printf("Parsing source file ... \n");
    }
    parse_source_file(decl_tree, context, root_node);
    if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
      printf("Parsing source file ... OK\n");
    }
    ast_ok = context.ok;
  }

  ts_tree_delete(tree);
  return ast_ok;
}

API::Result API::lpp(const std::vector<std::string>& lart_files, std::string& output_file) {
  std::string ll_file;
  if (output_file.ends_with(".bc")) {
    ll_file = generate_temp_file(".ll");
  } else {
    if (output_file.empty()) {
      output_file = generate_temp_file(".ll");
    }
    ll_file = output_file;
  }

  TSParser* parser = ts_parser_new();
  const TSLanguage* language = tree_sitter_lart();
  ts_parser_set_language(parser, language);

  Declaration* decl_tree = Declaration::New(declaration_t::MODULE_DECL);
  FileDB file_db;

  /* AST-PHASE */
  TSContext context = {
    .language = language,
    .source_code = nullptr,
    .filepath = nullptr,
    .file_db = &file_db,
    .file_queue = lart_files,
    .ok = true,
  };

  bool at_least_one_source_file = false;
  bool no_errors_occurred = true;

  while (!context.file_queue.empty()) {
    context.filepath = strclone(context.file_queue.back().c_str());
    context.file_queue.pop_back();

    if (std::filesystem::exists(context.filepath)) {
      no_errors_occurred &= parse_filepath(decl_tree, parser, context);
      at_least_one_source_file = true;
    } else {
      no_errors_occurred = false;
      std::cerr << RED_TEXT << "error" << NORMAL_TEXT << ": file '" << context.filepath << "' not found" << std::endl;
      break;
    }

    free((char*)context.filepath);
    context.filepath = nullptr;
  }

  if (!at_least_one_source_file) {
    std::cerr << RED_TEXT << "error" << NORMAL_TEXT << ": not source file specified" << std::endl;
    return Result::NO_SOURCE_FILE_SPECIFIED;
  }

  if (!no_errors_occurred) {
    return Result::PARSING_ERROR;
  }

  ts_parser_delete(parser);

  /* RESOLVE-PHASE */
  SymbolCache symbol_cache;
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Resolving symbols ... \n");
  }
  no_errors_occurred &= resolve_symbols(file_db, symbol_cache, decl_tree);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Resolving symbols ... OK\n");
  }

  if (!no_errors_occurred) {
    return Result::SYMBOL_RESOLUTION_ERROR;
  }

  /* DECL-TYPE-CHECK-PHASE */
  SizeCache size_cache;
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking declared types ... \n");
  }
  no_errors_occurred &= check_declared_types(file_db, symbol_cache, size_cache, decl_tree);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking declared types ... OK\n");
  }

  if (!no_errors_occurred) {
    return Result::DECLARED_TYPE_CHECKING_ERROR;
  }

  /* TYPE-CHECK-PHASE */
  TypeCache type_cache;
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking types ... \n");
  }
  no_errors_occurred &= check_types(file_db, symbol_cache, type_cache, decl_tree);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking types ... OK\n");
  }

  if (!no_errors_occurred) {
    return Result::TYPE_CHECKING_ERROR;
  }

  /* CONSTANT PROPAGATION */
  ConstantCache constant_cache;
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking constants ... \n");
  }
  no_errors_occurred &= check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl_tree);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Checking constants ... OK\n");
  }

  if (!no_errors_occurred) {
    return Result::CONSTANT_CHECKING_ERROR;
  }

  /* END-PHASE */
  if (API::DUMP_DEBUG_INFO_FOR_STRUCS) {
    std::filesystem::create_directories("tmp");
    print_to_file(decl_tree, "tmp/decl_tree.txt");
    print_to_file(symbol_cache, "tmp/symbol_cache.txt", file_db);
    print_to_file(file_db, "tmp/file_db.txt");
    print_to_file(type_cache, "tmp/type_cache.txt");
    print_to_file(size_cache, "tmp/size_cache.txt");
  }

  /* CODE-GEN-PHASE */
  LiteralStore literal_store;
  CGContext codegen_context = {
    .file_db = file_db,
    .symbol_cache = symbol_cache,
    .type_cache = type_cache,
    .size_cache = size_cache,
    .literal_store = literal_store
  };
  std::ofstream bucket (ll_file);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Emitting LLVM to %s ... \n", output_file.c_str());
  }
  emit_llvm(bucket, codegen_context, decl_tree);
  if (API::DEBUG_SEGFAULT_IDENTIFY_PHASE) {
    printf("Emitting LLVM to %s ... OK\n", output_file.c_str());
  }
  bucket.close();

  if (!no_errors_occurred) {
    return Result::LLVM_IR_GENERATION_ERROR;
  }

  if (output_file.ends_with(".bc")) {
    std::ostringstream cmd ("");
    cmd << "llvm-as " << ll_file << " -o " << output_file;
    if (execute_command_line(cmd.str()) == Result::OK) {
      return Result::OK;
    }
  }

  /* END-PHASE */
  if (API::DUMP_DEBUG_INFO_FOR_STRUCS) {
    std::filesystem::create_directories("tmp");
    print_to_file(decl_tree, "tmp/decl_tree.txt");
    print_to_file(symbol_cache, "tmp/symbol_cache.txt", file_db);
    print_to_file(file_db, "tmp/file_db.txt");
    print_to_file(type_cache, "tmp/type_cache.txt");
    print_to_file(size_cache, "tmp/size_cache.txt");
  }

  TypeCache::Delete(type_cache);
  ConstantCache::Delete(constant_cache);
  Declaration::Delete(decl_tree);
  FileDB::Delete(file_db);

  return Result::OK;
}
