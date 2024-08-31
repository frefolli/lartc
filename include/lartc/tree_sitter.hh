#ifndef LARTC_TREE_SITTER
#define LARTC_TREE_SITTER
#include <tree_sitter/api.h>
#include <lartc/internal_errors.hh>
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

inline void ts_validate_parsing(const TSLanguage* language, TSNode& node, const char* context, void* parsed) {
  if (parsed == nullptr) {
    const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (" << context << ")"));
  }
}
#endif//LARTC_TREE_SITTER
