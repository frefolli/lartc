#ifndef LARTC_AST_DECLARATION_PARSE
#define LARTC_AST_DECLARATION_PARSE
#include <lartc/ast/declaration.hh>
#include <lartc/tree_sitter.hh>
#include <tree_sitter/api.h>

Declaration* parse_declaration(TSContext& context, TSNode& node);
void parse_source_file(Declaration* decl_tree, TSContext& context, TSNode& root_node);
#endif//LARTC_AST_DECLARATION_PARSE
