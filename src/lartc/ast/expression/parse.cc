#include <lartc/ast/expression/parse.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/ast/parse.hh>
#include <lartc/internal_errors.hh>
#include <lartc/tree_sitter.hh>
#include <cstring>
#include <tree_sitter/api.h>
#include <unordered_map>

Expression* parse_expression_identifier(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* identifier = Expression::New(IDENTIFIER_EXPR);
  return identifier;
}

Expression* parse_expression_scoped_identifier(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* scoped_identifier = Expression::New(SCOPED_IDENTIFIER_EXPR);
  return scoped_identifier;
}

Expression* parse_expression_integer(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* integer = Expression::New(INTEGER_EXPR);
  return integer;
}

Expression* parse_expression_double(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* double_ = Expression::New(DOUBLE_EXPR);
  return double_;
}

Expression* parse_expression_boolean(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* boolean = Expression::New(BOOLEAN_EXPR);
  return boolean;
}

Expression* parse_expression_nullptr(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* nullptr_ = Expression::New(NULLPTR_EXPR);
  return nullptr_;
}

Expression* parse_expression_character(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* character = Expression::New(CHARACTER_EXPR);
  return character;
}

Expression* parse_expression_string(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* string = Expression::New(STRING_EXPR);
  return string;
}

Expression* parse_expression_call(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* call = Expression::New(CALL_EXPR);
  return call;
}

Expression* parse_expression_binary(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* binary = Expression::New(BINARY_EXPR);
  return binary;
}

Expression* parse_expression_monary(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* monary = Expression::New(MONARY_EXPR);
  return monary;
}

Expression* parse_expression_sizeof(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* sizeof_ = Expression::New(SIZEOF_EXPR);
  return sizeof_;
}

Expression* parse_expression_cast(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* cast = Expression::New(CAST_EXPR);
  return cast;
}

Expression* parse_expression_bitcast(const TSLanguage* language, const char* source_code, TSNode& node) {
  Expression* bitcast = Expression::New(BITCAST_EXPR);
  return bitcast;
}

typedef Expression*(*expression_parser)(const TSLanguage* language, const char* source_code, TSNode& node);
std::unordered_map<std::string, expression_parser> expression_parsers = {
  {"identifier", parse_expression_identifier},
  {"scoped_identifier", parse_expression_scoped_identifier},
  {"integer", parse_expression_integer},
  {"double", parse_expression_double},
  {"boolean", parse_expression_boolean},
  {"nullptr", parse_expression_nullptr},
  {"character", parse_expression_character},
  {"string", parse_expression_string},
  {"call_expression", parse_expression_call},
  {"binary_expression", parse_expression_binary},
  {"monary_expression", parse_expression_monary},
  {"sizeof_expression", parse_expression_sizeof},
  {"cast_expression", parse_expression_cast},
  {"bitcast_expression", parse_expression_bitcast},
};

Expression* parse_expression(const TSLanguage* language, const char* source_code, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
  auto it = expression_parsers.find(symbol_name);
  if (it != expression_parsers.end()) {
    return it->second(language, source_code, node);
  }
  return nullptr;
}
