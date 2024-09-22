#include <cstdio>
#include <cstdlib>
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
#include <lartc/typecheck/size_cache.hh>
#include <lartc/typecheck/check_declared_types.hh>
#include <lartc/codegen/cg_context.hh>
#include <lartc/codegen/emit_llvm.hh>

#include <assert.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <iostream>

#define DEBUG_SEGFAULT_IDENTIFY_PHASE

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

  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    context.source_code,
    strlen(context.source_code)
  );
  TSNode root_node = ts_tree_root_node(tree);
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Checking ts_tree for errors ... \n");
  #endif
  bool ast_ok = check_ts_tree_for_errors(context, root_node);
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Checking ts_tree for errors ... OK\n");
  #endif

  if (ast_ok) {
    context.ok = true;
    #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
    printf("Parsing source file ... \n");
    #endif
    parse_source_file(decl_tree, context, root_node);
    #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
    printf("Parsing source file ... OK\n");
    #endif
    ast_ok = context.ok;
  }

  ts_tree_delete(tree);
  return ast_ok;
}

char* strclone(const char* string) {
  int len = strlen(string);
  char* buf = (char*) malloc (len + 1);
  strcpy(buf, string);
  return buf;
}

namespace API {
  enum Result {
    
  };

  // If output_file.empty() then output_file = tmpdir.new_file()
  void lartc(const std::vector<std::string>& lart_files, std::string& output_file);
  // If output_file.empty() then output_file = tmpdir.new_file()
  void llc(const std::vector<std::string>& llvm_ir_files, std::string& output_file);
  // If output_file.empty() then output_file = tmpdir.new_file()
  void as(const std::vector<std::string>& asm_files, std::string& output_file);
  // If output_file.empty() then output_file = tmpdir.new_file()
  void ld(const std::vector<std::string>& object_files, std::string& output_file);
};

std::string generate_temp_file(std::string ext) {
  char buffer[L_tmpnam];
  std::tmpnam(buffer);
  std::string output_file = buffer;
  std::cout << output_file << std::endl;
  assert(!output_file.empty());
  output_file += ext;
  return output_file;
}

void API::lartc(const std::vector<std::string>& lart_files, std::string& output_file) {
  if (output_file.empty()) {
    output_file = generate_temp_file(".ll");
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
    } else {
      no_errors_occurred = false;
      std::cerr << RED_TEXT << "error" << NORMAL_TEXT << ": file '" << context.filepath << "' not found" << std::endl;
      break;
    }
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
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Resolving symbols ... \n");
  #endif
  no_errors_occurred &= resolve_symbols(file_db, symbol_cache, decl_tree);
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Resolving symbols ... OK\n");
  #endif

  if (!no_errors_occurred) {
    std::exit(2);
  }

  /* DECL-TYPE-CHECK-PHASE */
  SizeCache size_cache;
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Checking declared types ... \n");
  #endif
  no_errors_occurred &= check_declared_types(file_db, symbol_cache, size_cache, decl_tree);
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Checking declared types ... OK\n");
  #endif

  if (!no_errors_occurred) {
    std::exit(2);
  }

  /* TYPE-CHECK-PHASE */
  TypeCache type_cache;
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Checking types ... \n");
  #endif
  no_errors_occurred &= check_types(file_db, symbol_cache, type_cache, decl_tree);
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Checking types ... OK\n");
  #endif

  if (!no_errors_occurred) {
    std::exit(2);
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
  std::ofstream bucket (output_file);
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Emitting LLVM ... \n");
  #endif
  emit_llvm(bucket, codegen_context, decl_tree);
  #ifdef DEBUG_SEGFAULT_IDENTIFY_PHASE
  printf("Emitting LLVM ... OK\n");
  #endif
  bucket.close();

  if (!no_errors_occurred) {
    std::exit(2);
  }
  
  /* END-PHASE */
  std::filesystem::create_directories("tmp");
  print_to_file(decl_tree, "tmp/decl_tree.txt");
  print_to_file(symbol_cache, "tmp/symbol_cache.txt", file_db);
  print_to_file(file_db, "tmp/file_db.txt");
  print_to_file(type_cache, "tmp/type_cache.txt");
  print_to_file(size_cache, "tmp/size_cache.txt");

  Declaration::Delete(decl_tree);
  ts_parser_delete(parser);
}

void API::llc(const std::vector<std::string>& llvm_ir_files, std::string& output_file) {
  if (output_file.empty()) {
    output_file = generate_temp_file(".s");
  }

  std::ostringstream cmd ("");
  cmd << "llc";
  for (std::string llvm_ir_file : llvm_ir_files) {
    cmd << " " << llvm_ir_file;
  }
  cmd << " -o " << output_file;
  
  std::string cmd_line = cmd.str();
  int exit_code = system(cmd_line.c_str());
  assert(exit_code == 0);
}

void API::as(const std::vector<std::string>& asm_files, std::string& output_file) {
  if (output_file.empty()) {
    output_file = generate_temp_file(".o");
  }

  std::ostringstream cmd ("");
  cmd << "as";
  for (std::string asm_file : asm_files) {
    cmd << " " << asm_file;
  }
  cmd << " -o " << output_file;
  
  std::string cmd_line = cmd.str();
  int exit_code = system(cmd_line.c_str());
  assert(exit_code == 0);
}

void API::ld(const std::vector<std::string>& object_files, std::string& output_file) {
  if (output_file.empty()) {
    output_file = generate_temp_file(".exe");
  }

  std::ostringstream cmd ("");
  cmd << "/usr/bin/ld --hash-style=gnu --build-id --eh-frame-hdr -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crt1.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crti.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/crtbegin.o -L/usr/bin/../lib/clang/18/lib/x86_64-redhat-linux-gnu -L/usr/bin/../lib/gcc/x86_64-redhat-linux/14 -L/usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -L/lib -L/usr/lib -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /usr/bin/../lib/gcc/x86_64-redhat-linux/14/crtend.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crtn.o";
  for (std::string object_file : object_files) {
    cmd << " " << object_file;
  }
  cmd << " -o " << output_file;
  
  std::string cmd_line = cmd.str();
  int exit_code = system(cmd_line.c_str());
  assert(exit_code == 0);
}

int main(int argc, char** args) {
  std::vector<std::string> lart_files = {};
  std::vector<std::string> llvm_ir_files = {};
  std::vector<std::string> asm_files = {};
  std::vector<std::string> object_files = {};

  std::string object_file;
  std::string asm_file;
  std::string llvm_ir_file;
  std::string linked_file;
  std::string output;

  enum Workflow {
    DONT_COMPILE, // -E
    DONT_ASSEMBLE,// -S
    DONT_LINK,    // -c
    ALL
  } workflow;

  uint64_t n_of_args = argc;
  for (uint64_t i = 0; i < n_of_args; ++i) {
    std::string arg = args[i];
    if (arg == "-h" || arg == "--help") {
      // TODO: print help
    } else if (arg == "-E") {
      workflow = Workflow::DONT_COMPILE;
    } else if (arg == "-S") {
      workflow = Workflow::DONT_ASSEMBLE;
    } else if (arg == "-c") {
      workflow = Workflow::DONT_LINK;
    } else if (arg == "-o" || arg == "--output") {
      if (i + 1 < n_of_args) {
        output = args[i + 1];
        i += 1;
      } else {
        std::cerr << RED_TEXT << "error" << RED_TEXT << ": expected argument after '" << arg << "'" << std::endl;
        std::exit(1);
      }
    } else {
      std::string ext = std::filesystem::path(arg).extension();
      if (ext == ".lart") {
        lart_files.push_back(arg);
      } else if (ext == ".ll") {
        llvm_ir_files.push_back(arg);
      } else if (ext == ".s") {
        asm_files.push_back(arg);
      } else if (ext == ".o") {
        object_files.push_back(arg);
      } else {
        std::cerr << RED_TEXT << "error" << RED_TEXT << ": file '" << arg << "' has unknown extension" << std::endl;
        std::exit(1);
      }
    }
  }

  if (lart_files.size() > 0) {
    if (workflow == Workflow::DONT_COMPILE) {
      if (output.empty()) {
        output = "a.ll";
      }
      llvm_ir_file = output;
    }

    API::lartc(lart_files, llvm_ir_file);
    llvm_ir_files.push_back(llvm_ir_file);
  }


  if (workflow != Workflow::DONT_COMPILE) {
    if (llvm_ir_files.size() > 0) {
      if (workflow == Workflow::DONT_ASSEMBLE) {
        if (output.empty()) {
          output = "a.s";
        }
        asm_file = output;
      }

      API::llc(llvm_ir_files, asm_file);
      asm_files.push_back(asm_file);
    }
  }

  if (workflow != Workflow::DONT_ASSEMBLE) {
    if (asm_files.size() > 0) {
      if (workflow == Workflow::DONT_LINK) {
        if (output.empty()) {
          output = "a.o";
        }
        asm_file = output;
      }
      API::as(asm_files, object_file);
      object_files.push_back(object_file);
    }
  }

  if (workflow != Workflow::DONT_LINK) {
    if (output.empty()) {
      output = "a.exe";
    }
    asm_file = output;
    if (object_files.size() > 0) {
      API::ld(object_files, linked_file);
    }
  }
  return 0;
}
