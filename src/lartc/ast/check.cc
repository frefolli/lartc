#include <lartc/ast/check.hh>
#include <tree_sitter/api.h>
#include <cstring>
#include <iostream>

const char* RED_TEXT = "\x1b[1;31m";
const char* NORMAL_TEXT = "\x1b[0;39m";

uint64_t find_next_newline(const char* source_code, uint64_t byte_start) {
  while(source_code[byte_start] != '0' && source_code[byte_start] != '\n')
    byte_start++;
  return byte_start;
}

void syntax_error(TSPoint& point, const char* node_symbol_name, const char* source_code, uint64_t byte_start) {
  const char* filepath = "wrong.lart"; // TODO: build a context struct for passing multiple source files with each their filepath
  std::cerr << filepath << ":" << point.row+1 << ":" << point.column+1 << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unexpected token inside of " << node_symbol_name << std::endl;

  uint64_t byte_of_first_character_of_line = byte_start - point.column;
  uint64_t byte_of_last_character_of_line = find_next_newline(source_code, byte_start);
  std::cerr << std::string(source_code + byte_of_first_character_of_line, byte_of_last_character_of_line - byte_of_first_character_of_line) << std::endl;
  std::cout << std::string(point.column, ' ') << "^" << std::endl; 
}

bool check_ts_tree_for_errors(const TSLanguage* language, const char* source_code, TSNode& node) {
  bool ok = true;
  const char* node_symbol_name = ts_language_symbol_name(language, ts_node_symbol(node));
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    const char* child_node_symbol_name = ts_language_symbol_name(language, ts_node_symbol(child_node));
    if (std::strcmp(child_node_symbol_name, "ERROR") == 0) {
      TSPoint child_start = ts_node_start_point(child_node);
      syntax_error(child_start, node_symbol_name, source_code, ts_node_start_byte(child_node));
      ok = false;
      check_ts_tree_for_errors(language, source_code, child_node);
    } else {
      ok &= check_ts_tree_for_errors(language, source_code, child_node);
    }
  }
  return ok;
}
