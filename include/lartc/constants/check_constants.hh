#ifndef LARTC_CONSTANTS_CHECK_CONSTANTS
#define LARTC_CONSTANTS_CHECK_CONSTANTS
#include <lartc/ast/declaration.hh>
#include <lartc/ast/file_db.hh>
#include <lartc/resolve/symbol_cache.hh>
#include <lartc/typecheck/size_cache.hh>
#include <lartc/typecheck/type_cache.hh>
#include <lartc/constants/constant_cache.hh>

bool check_constants(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, TypeCache& type_cache, ConstantCache& constant_cache, Declaration* decl_tree);
#endif//LARTC_CONSTANTS_CHECK_CONSTANTS
