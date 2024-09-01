#ifndef LARTC_AST_TYPE_PARSE
#define LARTC_AST_TYPE_PARSE
#include <lartc/ast/type.hh>
#include <lartc/tree_sitter.hh>
#include <tree_sitter/api.h>

Type* parse_type(TSContext& context, TSNode& node);
#endif//LARTC_AST_TYPE_PARSE
