#ifndef LARTC_AST_CHECK
#define LARTC_AST_CHECK
#include <tree_sitter/api.h>

bool check_ts_tree_for_errors(const TSLanguage* language, const char* source_code, TSNode& node);
#endif//LARTC_AST_CHECK
