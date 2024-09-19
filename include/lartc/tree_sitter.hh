#ifndef LARTC_TREE_SITTER
#define LARTC_TREE_SITTER
#include <tree_sitter/api.h>
#include <lartc/internal_errors.hh>
#include <lartc/ast/file_db.hh>
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
    TSPoint start = ts_node_start_point(node);
    TSPoint end = ts_node_end_point(node);
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(
          ": " << std::string(symbol_name)
          << " inside a (" << context << ")"
          << " from " << start.row << ":" << start.column
          << " to " << end.row << ":" << end.column
    ));
  }
}

struct TSContext {
  const TSLanguage* language;
  const char* source_code;
  const char* filepath;
  FileDB* file_db;
  std::vector<std::string> file_queue;
  bool ok;
};
#endif//LARTC_TREE_SITTER
