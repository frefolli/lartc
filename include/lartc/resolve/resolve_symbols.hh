#ifndef LARTC_RESOLVE_RESOLVE_SYMBOLS
#define LARTC_RESOLVE_RESOLVE_SYMBOLS
#include <lartc/resolve/symbol_cache.hh>
#include <lartc/ast/file_db.hh>

bool resolve_symbols(FileDB& file_db, SymbolCache& symbol_cache, Declaration* decl);
#endif//LARTC_RESOLVE_RESOLVE_SYMBOLS
