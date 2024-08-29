#ifndef LARTC_AST_TYPE_PARSE
#define LARTC_AST_TYPE_PARSE
#include <lartc/ast/type.hh>
#include <tree_sitter/api.h>

Type* parse_type(const TSLanguage* language, const char* source_code, TSNode& node);
#endif//LARTC_AST_TYPE_PARSE
