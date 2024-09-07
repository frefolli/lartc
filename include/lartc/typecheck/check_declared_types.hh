#ifndef LARTC_AST_TYPECHECK_CHECK_DECLARED_TYPES
#define LARTC_AST_TYPECHECK_CHECK_DECLARED_TYPES
#include <lartc/ast/file_db.hh>
#include <lartc/resolve/symbol_cache.hh>
#include <lartc/typecheck/size_cache.hh>
#include <lartc/ast/declaration.hh>

bool check_declared_types(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, Declaration* decl_tree);
#endif//LARTC_AST_TYPECHECK_CHECK_DECLARED_TYPES
