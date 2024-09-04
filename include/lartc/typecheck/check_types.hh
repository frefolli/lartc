#ifndef LARTC_AST_TYPECHECK_CHECK_TYPES
#define LARTC_AST_TYPECHECK_CHECK_TYPES
#include <lartc/ast/file_db.hh>
#include <lartc/resolve/symbol_cache.hh>
#include <lartc/typecheck/type_cache.hh>
#include <lartc/ast/declaration.hh>

bool check_types(FileDB& file_db, SymbolCache& symbol_cache, TypeCache& type_cache, Declaration* decl_tree);
#endif//LARTC_AST_TYPECHECK_CHECK_TYPES

