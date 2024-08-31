#ifndef LARTC_AST_STATEMENT_PARSE
#define LARTC_AST_STATEMENT_PARSE
#include <lartc/ast/statement.hh>
#include <tree_sitter/api.h>

Statement* parse_statement(const TSLanguage* language, const char* source_code, TSNode& node);
#endif//LARTC_AST_STATEMENT_PARSE
