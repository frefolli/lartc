#ifndef LARTC_AST_EXPRESSION_PARSE
#define LARTC_AST_EXPRESSION_PARSE
#include <lartc/ast/expression.hh>
#include <lartc/tree_sitter.hh>
#include <tree_sitter/api.h>

Expression* parse_expression(TSContext& context, TSNode& node);
#endif//LARTC_AST_EXPRESSION_PARSE
