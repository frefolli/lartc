#include <lartc/ast/check.hh>
#include <tree_sitter/api.h>
#include <cstring>
#include <iostream>

bool check_ts_tree_for_errors(const TSLanguage* language, const char* source_code, TSNode& node) {
  bool ok = true;
  const char* node_symbol_name = ts_language_symbol_name(language, ts_node_symbol(node));
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    const char* child_node_symbol_name = ts_language_symbol_name(language, ts_node_symbol(child_node));
    if (std::strcmp(child_node_symbol_name, "ERROR") == 0) {
      TSPoint child_start = ts_node_start_point(child_node);
      std::cerr << "syntax error: unexpected token at " << child_start.row << ":" << child_start.column << " inside of " << node_symbol_name << std::endl;
      ok = false;
    } else {
      ok &= check_ts_tree_for_errors(language, source_code, child_node);
    }
  }
  return ok;
}
