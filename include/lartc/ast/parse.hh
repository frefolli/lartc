#ifndef LARTC_AST_PARSE
#define LARTC_AST_PARSE
#include <vector>
#include <utility>
#include <lartc/ast/type.hh>
#include <lartc/tree_sitter.hh>
#include <tree_sitter/api.h>

std::pair<std::vector<std::pair<std::string, Type*>>, bool> parse_field_parameter_list(TSContext& context, TSNode& node);
#endif//LARTC_AST_PARSE
