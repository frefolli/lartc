#ifndef LARTC_TYPECHECK_SIZE_CACHE
#define LARTC_TYPECHECK_SIZE_CACHE
#include <lartc/ast/declaration.hh>
#include <lartc/resolve/symbol_cache.hh>
#include <map>
#include <cstdint>

struct SizeCache {
  std::map<Declaration*, uint64_t> sizes;
  std::map<Declaration*, bool> staging;

  void add_declaration(SymbolCache& symbol_cache, Declaration* decl);
  uint64_t compute_size(SymbolCache& symbol_cache, Declaration* context, Type* type);
};
#endif//LARTC_TYPECHECK_SIZE_CACHE
