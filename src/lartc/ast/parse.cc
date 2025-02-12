#include <lartc/ast/parse.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/tree_sitter.hh>
#include <lartc/internal_errors.hh>

std::pair<std::vector<std::pair<std::string, Type*>>, bool> parse_field_parameter_list(TSContext& context, TSNode& node) {
  std::vector<std::pair<std::string, Type*>> list = {};
  bool is_variadic = false;
  std::uintmax_t child_count = ts_node_named_child_count(node);
  for (std::uintmax_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(child_node));
    if (strcmp(symbol_name, "variadic_parameter") == 0) {
      is_variadic = true;
    } else {
      if (!ts_can_ignore(symbol_name)) {
        std::pair<std::string, Type*> item;
        TSNode name = ts_node_child_by_field_name(child_node, "name");
        TSNode type = ts_node_child_by_field_name(child_node, "type");

        if (name.id != nullptr && type.id != nullptr) {
          item.first = ts_node_source_code(name, context.source_code);
          item.second = parse_type(context, type);

          if (item.second == nullptr) {
            const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(type));
            throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (field-paramterer-list) as type"));
          }
          list.push_back(item);
        } else {
          throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (field-paramterer-list)"));
        } 
      }
    }
  }
  return {list, is_variadic};
}
