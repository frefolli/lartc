#include <lartc/ast/parse.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/internal_errors.hh>
#include <lartc/tree_sitter.hh>
#include <ios>
#include <cstring>
#include <tree_sitter/api.h>
#include <unordered_map>
#include <utility>
#include <cassert>
#include <iostream>

Type* parse_type_integer(TSContext& context, TSNode& node) {
  Type* type = Type::New(type_t::INTEGER_TYPE);

  TSNode size = ts_node_child_by_field_name(node, "size");
  type->size = std::stoi(ts_node_source_code(size, context.source_code));

  TSNode is_signed = ts_node_child_by_field_name(node, "signed");
  std::stringstream(ts_node_source_code(is_signed, context.source_code)) >> std::boolalpha >> type->is_signed >> std::noboolalpha;

  return type;
}

Type* parse_type_double(TSContext& context, TSNode& node) {
  Type* type = Type::New(type_t::DOUBLE_TYPE);

  TSNode size = ts_node_child_by_field_name(node, "size");
  type->size = std::stoi(ts_node_source_code(size, context.source_code));

  return type;
}

Type* parse_type_boolean(TSContext& /*context*/, TSNode& /*node*/) {
  Type* type = Type::New(type_t::BOOLEAN_TYPE);

  return type;
}

Type* parse_type_pointer(TSContext& context, TSNode& node) {
  Type* type = Type::New(type_t::POINTER_TYPE);

  TSNode subtype = ts_node_child_by_field_name(node, "type");
  type->subtype = parse_type(context, subtype);
  if (type->subtype == nullptr) {
    const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(subtype));
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (pointer)"));
  }
  return type;
}

Type* parse_type_symbol(TSContext& context, TSNode& node) {
  Type* type = Type::New(type_t::SYMBOL_TYPE);
  type->symbol = Symbol::From(ts_node_source_code(node, context.source_code));
  context.file_db->add_symbol(&type->symbol, node);
  return type;
}

Type* parse_type_void(TSContext& /*context*/, TSNode& /*node*/) {
  Type* type = Type::New(type_t::VOID_TYPE);
  return type;
}

Type* parse_type_struct(TSContext& context, TSNode& node) {
  Type* type = Type::New(type_t::STRUCT_TYPE);
  
  TSNode fields = ts_node_child_by_field_name(node, "fields");
  type->fields = parse_field_parameter_list(context, fields);

  return type;
}

Type* parse_type_function(TSContext& context, TSNode& node) {
  Type* type = Type::New(type_t::FUNCTION_TYPE);
  
  TSNode parameters = ts_node_child_by_field_name(node, "parameters");
  type->parameters = parse_field_parameter_list(context, parameters);

  TSNode subtype = ts_node_child_by_field_name(node, "type");
  if (subtype.id) {
    type->subtype = parse_type(context, subtype);
  } else {
    type->subtype = Type::New(type_t::VOID_TYPE);
  }

  return type;
}

typedef Type*(*type_parser)(TSContext& context, TSNode& node);
std::unordered_map<std::string, type_parser> type_parsers = {
  {"integer_type", parse_type_integer},
  {"double_type", parse_type_double},
  {"boolean_type", parse_type_boolean},
  {"pointer_type", parse_type_pointer},
  {"identifier", parse_type_symbol},
  {"scoped_identifier", parse_type_symbol},
  {"void_type", parse_type_void},
  {"struct_type", parse_type_struct},
  {"function_type", parse_type_function},
};

Type* parse_type(TSContext& context, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(node));
  auto it = type_parsers.find(symbol_name);
  if (it != type_parsers.end()) {
    Type* type = it->second(context, node);
    if (type != nullptr) {
      context.file_db->add_type(type, node);
    }
    return type;
  }
  return nullptr;
}
