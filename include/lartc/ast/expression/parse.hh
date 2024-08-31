#ifndef LARTC_AST_EXPRESSION_PARSE
#define LARTC_AST_EXPRESSION_PARSE
#include <lartc/ast/expression.hh>
#include <tree_sitter/api.h>

Expression* parse_expression(const TSLanguage* language, const char* source_code, TSNode& node);
#endif//LARTC_AST_EXPRESSION_PARSE
