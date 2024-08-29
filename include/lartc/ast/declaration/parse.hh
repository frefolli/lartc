#ifndef LARTC_AST_DECLARATION_PARSE
#define LARTC_AST_DECLARATION_PARSE
#include <lartc/ast/declaration.hh>
#include <tree_sitter/api.h>

Declaration* parse_declaration(const TSLanguage* language, const char* source_code, TSNode& node);
Declaration* parse_source_file(const TSLanguage* language, const char* source_code, TSNode& root_node);
#endif//LARTC_AST_DECLARATION_PARSE
