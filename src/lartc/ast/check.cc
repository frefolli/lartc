#include <lartc/ast/check.hh>
#include <lartc/external_errors.hh>
#include <tree_sitter/api.h>
#include <cstring>

bool check_ts_tree_for_errors(TSContext& context, TSNode& node) {
  bool ok = true;
  const char* node_symbol_name = ts_language_symbol_name(context.language, ts_node_symbol(node));
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    const char* child_node_symbol_name = ts_language_symbol_name(context.language, ts_node_symbol(child_node));
    if (std::strcmp(child_node_symbol_name, "ERROR") == 0) {
      TSPoint child_start = ts_node_start_point(child_node);
      throw_syntax_error(context.filepath, child_start, node_symbol_name, context.source_code, ts_node_start_byte(child_node));
      ok = false;
      check_ts_tree_for_errors(context, child_node);
    } else {
      ok &= check_ts_tree_for_errors(context, child_node);
    }
  }
  return ok;
}
