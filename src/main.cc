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

void tabulate(uint64_t tabulation) {
  for (uint64_t i = 0; i < tabulation; ++i) {
    std::cout << "  ";
  }
}

void visit_node(const TSLanguage* language, const char* text, TSNode& node, uint64_t tabulation = 0) {
  const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
  tabulate(tabulation);
  uint64_t child_count = ts_node_named_child_count(node);
  std::cout << symbol_name << std::endl;
  for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child = ts_node_named_child(node, child_index);
    visit_node(language, text, child, tabulation + 1);
  }
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
  visit_node(lart, source_code, root_node);

  ts_tree_delete(tree);
  ts_parser_delete(parser);
  free(source_code);
  return 0;
}
