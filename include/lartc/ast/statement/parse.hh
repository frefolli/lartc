#ifndef LARTC_AST_STATEMENT_PARSE
#define LARTC_AST_STATEMENT_PARSE
#include <lartc/ast/statement.hh>
#include <lartc/tree_sitter.hh>
#include <tree_sitter/api.h>

Statement* parse_statement(TSContext& context, TSNode& node);
#endif//LARTC_AST_STATEMENT_PARSE
