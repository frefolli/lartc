#include <lartc/ast/parse.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/tree_sitter.hh>
#include <lartc/internal_errors.hh>

std::vector<std::pair<std::string, Type*>> parse_field_parameter_list(const TSLanguage* language, const char* source_code, TSNode& node) {
  std::vector<std::pair<std::string, Type*>> list = {};
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    std::pair<std::string, Type*> item;
    TSNode name = ts_node_child_by_field_name(child_node, "name");
    item.first = ts_node_source_code(name, source_code);

    TSNode type = ts_node_child_by_field_name(child_node, "type");
    item.second = parse_type(language, source_code, type);

    if (item.second == nullptr) {
      const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(type));
      throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (field-paramterer-list)"));
    }
    list.push_back(item);
  }
  // return {};
  return list;
}
