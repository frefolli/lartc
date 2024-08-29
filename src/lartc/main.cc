#include <lartc/internal_errors.hh>
#include <lartc/ast/declaration.hh>
#include <lartc/ast/declaration/parse.hh>

#include <assert.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <iostream>

extern "C" const TSLanguage *tree_sitter_lart(void);

char* read_source_code(int argc, char** args) {
    char* text = NULL;
    const char* filepath = "hello.lart";
    if (argc > 1) {
        filepath = args[1];
    }
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = (char*) malloc(fsize + 1);
    fread(text, fsize, 1, file);
    fclose(file);
    return text;
}

int main(int argc, char** args) {
  TSParser* parser = ts_parser_new();
  const TSLanguage* lart =  tree_sitter_lart();
  ts_parser_set_language(parser, lart);
  char *source_code = read_source_code(argc, args);
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );
  TSNode root_node = ts_tree_root_node(tree);

  Declaration* source_file = parse_source_file(lart, source_code, root_node);
  Declaration::Print(std::clog, source_file);
  Declaration::Delete(source_file);

  ts_tree_delete(tree);
  ts_parser_delete(parser);
  free(source_code);
  return 0;
}
