#include <lartc/ast/declaration.hh>
#include <lartc/ast/declaration/parse.hh>
#include <lartc/ast/check.hh>
#include <lartc/tree_sitter.hh>
#include <lartc/terminal.hh>

#include <assert.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <iostream>

extern "C" const TSLanguage *tree_sitter_lart(void);

char* read_source_code(const char* filepath) {
    char* text = NULL;
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = (char*) malloc(fsize + 1);
    fread(text, fsize, 1, file);
    fclose(file);
    return text;
}

bool parse_filepath(Declaration* decl_tree, TSParser* parser, TSContext& context) {
  context.source_code = read_source_code(context.filepath);
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
  free((char*) context.source_code);

  return ast_ok;
}

int main(int argc, char** args) {
  TSParser* parser = ts_parser_new();
  const TSLanguage* language = tree_sitter_lart();
  ts_parser_set_language(parser, language); 

  Declaration* decl_tree = Declaration::New(declaration_t::MODULE_DECL);

  TSContext context = {
    .language = language,
    .source_code = nullptr,
    .filepath = nullptr
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

  Declaration::Print(std::clog, decl_tree) << std::endl;
  Declaration::Delete(decl_tree);

  ts_parser_delete(parser);
  return 0;
}
