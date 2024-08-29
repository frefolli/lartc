#ifndef LARTC_TREE_SITTER
#define LARTC_TREE_SITTER
#include <tree_sitter/api.h>
#include <cstring>
#include <string>

inline std::string ts_node_source_code(TSNode& node, const char* source_code) {
  return std::string(source_code + ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node));
}

inline TSNode ts_node_child_by_field_name(TSNode self, const char *name) {
  return ts_node_child_by_field_name(self, name, std::strlen(name));
}

inline bool ts_can_ignore(const char* symbol_name) {
  return (std::strcmp(symbol_name, "line_comment") == 0
       || std::strcmp(symbol_name, "block_comment") == 0);
}
#endif//LARTC_TREE_SITTER
