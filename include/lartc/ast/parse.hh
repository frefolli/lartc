#ifndef LARTC_AST_PARSE
#define LARTC_AST_PARSE
#include <vector>
#include <utility>
#include <lartc/ast/type.hh>
#include <tree_sitter/api.h>

std::vector<std::pair<std::string, Type*>> parse_field_parameter_list(const TSLanguage* language, const char* source_code, TSNode& node);
#endif//LARTC_AST_PARSE
