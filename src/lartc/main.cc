#include <filesystem>
#include <fstream>
#include <lartc/ast/declaration.hh>
#include <lartc/ast/declaration/parse.hh>
#include <lartc/ast/check.hh>
#include <lartc/resolve/resolve_symbols.hh>
#include <lartc/tree_sitter.hh>
#include <lartc/terminal.hh>
#include <lartc/ast/file_db.hh>
#include <lartc/typecheck/type_cache.hh>
#include <lartc/typecheck/check_types.hh>

#include <assert.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <iostream>

extern "C" const TSLanguage *tree_sitter_lart(void);

template<typename T>
void print_to_file(T& object, const char* filepath) {
  std::ofstream out (filepath);
  T::Print(out, object);
  out.close();
}

template<typename T>
void print_to_file(T* object, const char* filepath) {
  std::ofstream out (filepath);
  T::Print(out, object);
  out.close();
}

bool parse_filepath(Declaration* decl_tree, TSParser* parser, TSContext& context) {
  FileDB::File* file = context.file_db->add_file(context.filepath);
  context.source_code = file->source_code;

  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    context.source_code,
    strlen(context.source_code)
  );
  TSNode root_node = ts_tree_root_node(tree);
  bool ast_ok = check_ts_tree_for_errors(context, root_node);

  if (ast_ok) {
    parse_source_file(decl_tree, context, root_node);
  }

  ts_tree_delete(tree);
  return ast_ok;
}

int main(int argc, char** args) {
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
    .file_db = &file_db
  };
  
  bool at_least_one_source_file = false;
  bool no_errors_occurred = true;

  for (uint64_t arg_index = 1; arg_index < argc; ++arg_index) {
    context.filepath = args[arg_index];
    no_errors_occurred &= parse_filepath(decl_tree, parser, context);
    at_least_one_source_file = true;
  }

  if (!at_least_one_source_file) {
    std::cerr << RED_TEXT << "error" << NORMAL_TEXT << ": not source file specified" << std::endl;
    std::exit(1);
  }

  if (!no_errors_occurred) {
    std::exit(2);
  }

  /* RESOLVE-PHASE */
  SymbolCache symbol_cache;
  no_errors_occurred &= resolve_symbols(file_db, symbol_cache, decl_tree);

  if (!no_errors_occurred) {
    std::exit(2);
  }

  /* TYPE-CHECK-PHASE */
  TypeCache type_cache;
  no_errors_occurred &= check_types(file_db, symbol_cache, type_cache, decl_tree);
  
  std::filesystem::create_directories("tmp");
  print_to_file(decl_tree, "tmp/decl_tree.txt");
  print_to_file(symbol_cache, "tmp/symbol_cache.txt");
  print_to_file(file_db, "tmp/file_db.txt");
  print_to_file(type_cache, "tmp/type_cache.txt");
  
  /* END-PHASE */

  Declaration::Delete(decl_tree);

  ts_parser_delete(parser);
  return 0;
}
