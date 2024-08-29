#include <ios>
#include <lartc/ast/type/parse.hh>
#include <lartc/internal_errors.hh>
#include <lartc/tree_sitter.hh>
#include <cstring>
#include <tree_sitter/api.h>
#include <unordered_map>
#include <vector>
#include <utility>
#include <cassert>
#include <iostream>

Type* parse_type_integer(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::INTEGER_TYPE);

  TSNode size = ts_node_child_by_field_name(node, "size");
  type->size = std::stoi(ts_node_source_code(size, source_code));

  TSNode is_signed = ts_node_child_by_field_name(node, "signed");
  std::stringstream(ts_node_source_code(is_signed, source_code)) >> std::boolalpha >> type->is_signed >> std::noboolalpha;

  return type;
}

Type* parse_type_double(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::DOUBLE_TYPE);

  TSNode size = ts_node_child_by_field_name(node, "size");
  type->size = std::stoi(ts_node_source_code(size, source_code));

  return type;
}

Type* parse_type_boolean(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::BOOLEAN_TYPE);

  return type;
}

Type* parse_type_pointer(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::POINTER_TYPE);

  TSNode subtype = ts_node_child_by_field_name(node, "type");
  type->subtype = parse_type(language, source_code, subtype);
  if (type->subtype == nullptr) {
    const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(subtype));
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (pointer)"));
  }
  return type;
}

Type* parse_type_identifier(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::IDENTIFIER_TYPE);
  type->identifier = ts_node_source_code(node, source_code);
  return type;
}

Type* parse_type_void(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::VOID_TYPE);
  return type;
}

inline std::vector<std::pair<std::string, Type*>> parse_field_parameter_list(const TSLanguage* language, const char* source_code, TSNode& node) {
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

Type* parse_type_struct(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::STRUCT_TYPE);
  
  TSNode fields = ts_node_child_by_field_name(node, "fields");
  type->fields = parse_field_parameter_list(language, source_code, fields);

  return type;
}

Type* parse_type_function(const TSLanguage* language, const char* source_code, TSNode& node) {
  Type* type = Type::New(type_t::FUNCTION_TYPE);
  
  TSNode parameters = ts_node_child_by_field_name(node, "parameters");
  type->parameters = parse_field_parameter_list(language, source_code, parameters);

  TSNode subtype = ts_node_child_by_field_name(node, "type");
  if (subtype.id) {
    type->subtype = parse_type(language, source_code, subtype);
  } else {
    type->subtype = Type::New(type_t::VOID_TYPE);
  }

  return type;
}

typedef Type*(*type_parser)(const TSLanguage* language, const char* source_code, TSNode& node);
std::unordered_map<std::string, type_parser> type_parsers = {
  {"integer_type", parse_type_integer},
  {"double_type", parse_type_double},
  {"boolean_type", parse_type_boolean},
  {"pointer_type", parse_type_pointer},
  {"identifier", parse_type_identifier},
  {"scoped_identifier", parse_type_identifier},
  {"void_type", parse_type_void},
  {"struct_type", parse_type_struct},
  {"function_type", parse_type_function},
};

Type* parse_type(const TSLanguage* language, const char* source_code, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
  auto it = type_parsers.find(symbol_name);
  if (it != type_parsers.end()) {
    return it->second(language, source_code, node);
  }
  return nullptr;
}
