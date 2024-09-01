#include <lartc/ast/check.hh>
#include <lartc/terminal.hh>
#include <tree_sitter/api.h>
#include <cstring>
#include <iostream>

uint64_t find_next_newline(const char* source_code, uint64_t byte_start) {
  while(source_code[byte_start] != '0' && source_code[byte_start] != '\n')
    byte_start++;
  return byte_start;
}

void syntax_error(const char* filepath, TSPoint& point, const char* node_symbol_name, const char* source_code, uint64_t byte_start) {
  std::cerr << filepath << ":" << point.row+1 << ":" << point.column+1 << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unexpected token inside of " << node_symbol_name << std::endl;

  uint64_t byte_of_first_character_of_line = byte_start - point.column;
  uint64_t byte_of_last_character_of_line = find_next_newline(source_code, byte_start);
  std::cerr << std::string(source_code + byte_of_first_character_of_line, byte_of_last_character_of_line - byte_of_first_character_of_line) << std::endl;
  std::cout << std::string(point.column, ' ') << "^" << std::endl; 
}

bool check_ts_tree_for_errors(TSContext& context, TSNode& node) {
  bool ok = true;
  const char* node_symbol_name = ts_language_symbol_name(context.language, ts_node_symbol(node));
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    const char* child_node_symbol_name = ts_language_symbol_name(context.language, ts_node_symbol(child_node));
    if (std::strcmp(child_node_symbol_name, "ERROR") == 0) {
      TSPoint child_start = ts_node_start_point(child_node);
      syntax_error(context.filepath, child_start, node_symbol_name, context.source_code, ts_node_start_byte(child_node));
      ok = false;
      check_ts_tree_for_errors(context, child_node);
    } else {
      ok &= check_ts_tree_for_errors(context, child_node);
    }
  }
  return ok;
}
