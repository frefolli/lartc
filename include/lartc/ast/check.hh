#ifndef LARTC_AST_CHECK
#define LARTC_AST_CHECK
#include <tree_sitter/api.h>
#include <lartc/tree_sitter.hh>

bool check_ts_tree_for_errors(TSContext& context, TSNode& node);
#endif//LARTC_AST_CHECK
