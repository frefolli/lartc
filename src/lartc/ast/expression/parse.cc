#include <lartc/ast/expression/parse.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/ast/parse.hh>
#include <lartc/internal_errors.hh>
#include <lartc/tree_sitter.hh>
#include <cstring>
#include <tree_sitter/api.h>
#include <unordered_map>

Expression* parse_expression_identifier(TSContext& context, TSNode& node) {
  Expression* identifier = Expression::New(IDENTIFIER_EXPR);
  identifier->name = ts_node_source_code(node, context.source_code);
  return identifier;
}

Expression* parse_expression_scoped_identifier(TSContext& context, TSNode& node) {
  Expression* scoped_identifier = Expression::New(SCOPED_IDENTIFIER_EXPR);
  scoped_identifier->name = ts_node_source_code(node, context.source_code);
  return scoped_identifier;
}

Expression* parse_expression_integer(TSContext& context, TSNode& node) {
  Expression* integer = Expression::New(INTEGER_EXPR);
  integer->literal = ts_node_source_code(node, context.source_code);
  return integer;
}

Expression* parse_expression_double(TSContext& context, TSNode& node) {
  Expression* double_ = Expression::New(DOUBLE_EXPR);
  double_->literal = ts_node_source_code(node, context.source_code);
  return double_;
}

Expression* parse_expression_boolean(TSContext& context, TSNode& node) {
  Expression* boolean = Expression::New(BOOLEAN_EXPR);
  boolean->literal = ts_node_source_code(node, context.source_code);
  return boolean;
}

Expression* parse_expression_nullptr(TSContext& context, TSNode& node) {
  Expression* nullptr_ = Expression::New(NULLPTR_EXPR);
  return nullptr_;
}

Expression* parse_expression_character(TSContext& context, TSNode& node) {
  Expression* character = Expression::New(CHARACTER_EXPR);
  // TODO: Escape string
  character->literal = ts_node_source_code(node, context.source_code);
  return character;
}

Expression* parse_expression_string(TSContext& context, TSNode& node) {
  Expression* string = Expression::New(STRING_EXPR);
  // TODO: Escape string
  string->literal = ts_node_source_code(node, context.source_code);
  return string;
}

Expression* parse_expression_call(TSContext& context, TSNode& node) {
  Expression* call = Expression::New(CALL_EXPR);
  TSNode callable = ts_node_child_by_field_name(node, "callable");
  call->callable = parse_expression(context, callable);
  ts_validate_parsing(context.language, callable, "call_expr:callable", call->callable);

  TSNode arguments = ts_node_child_by_field_name(node, "arguments");
  uint64_t argument_count = ts_node_named_child_count(arguments);
  for (uint64_t argument_index = 0; argument_index < argument_count; ++argument_index) {
    TSNode argument_node = ts_node_named_child(arguments, argument_index);
    Expression* argument = parse_expression(context, argument_node);
    if (argument != nullptr) {
      call->arguments.push_back(argument);
    } else {
      const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(argument_node));
      if (!ts_can_ignore(symbol_name)) {
        throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (call_expr:arguments)"));
      }
    }
  }
  return call;
}

Expression* parse_expression_binary(TSContext& context, TSNode& node) {
  Expression* binary = Expression::New(BINARY_EXPR);
  TSNode left = ts_node_child_by_field_name(node, "left");
  binary->left = parse_expression(context, left);
  ts_validate_parsing(context.language, left, "binary_expr:left", binary->left);

  TSNode right = ts_node_child_by_field_name(node, "right");
  binary->right = parse_expression(context, right);
  ts_validate_parsing(context.language, right, "binary_expr:right", binary->right);

  TSNode operator_ = ts_node_child_by_field_name(node, "operator");
  binary->operator_ = ts_node_source_code(operator_, context.source_code);
  return binary;
}

Expression* parse_expression_monary(TSContext& context, TSNode& node) {
  Expression* monary = Expression::New(MONARY_EXPR);
  TSNode value = ts_node_child_by_field_name(node, "value");
  monary->value = parse_expression(context, value);
  ts_validate_parsing(context.language, value, "monary_expr:value", monary->value);

  TSNode operator_ = ts_node_child_by_field_name(node, "operator");
  monary->operator_ = ts_node_source_code(operator_, context.source_code);
  return monary;
}

Expression* parse_expression_sizeof(TSContext& context, TSNode& node) {
  Expression* sizeof_ = Expression::New(SIZEOF_EXPR);
  TSNode type = ts_node_child_by_field_name(node, "type");
  sizeof_->type = parse_type(context, type);
  ts_validate_parsing(context.language, type, "sizeof_expr:type", sizeof_->type);
  return sizeof_;
}

Expression* parse_expression_cast(TSContext& context, TSNode& node) {
  Expression* cast = Expression::New(CAST_EXPR);
  TSNode type = ts_node_child_by_field_name(node, "type");
  cast->type = parse_type(context, type);
  ts_validate_parsing(context.language, type, "cast_expr:type", cast->type);

  TSNode value = ts_node_child_by_field_name(node, "value");
  cast->value = parse_expression(context, value);
  ts_validate_parsing(context.language, value, "cast_expr:value", cast->value);
  return cast;
}

Expression* parse_expression_bitcast(TSContext& context, TSNode& node) {
  Expression* bitcast = Expression::New(BITCAST_EXPR);
  TSNode type = ts_node_child_by_field_name(node, "type");
  bitcast->type = parse_type(context, type);
  ts_validate_parsing(context.language, type, "bitcast_expr:type", bitcast->type);

  TSNode value = ts_node_child_by_field_name(node, "value");
  bitcast->value = parse_expression(context, value);
  ts_validate_parsing(context.language, value, "bitcast_expr:value", bitcast->value);
  return bitcast;
}

inline Expression* parse_expression_parenthesized(TSContext& context, TSNode& node) {
  TSNode inner_expr = ts_node_named_child(node, 0);
  return parse_expression(context, inner_expr);
}

typedef Expression*(*expression_parser)(TSContext& context, TSNode& node);
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
  {"parenthesized_expression", parse_expression_parenthesized},
};

Expression* parse_expression(TSContext& context, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(node));
  auto it = expression_parsers.find(symbol_name);
  if (it != expression_parsers.end()) {
    return it->second(context, node);
  }
  return nullptr;
}
