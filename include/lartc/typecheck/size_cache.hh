#ifndef LARTC_TYPECHECK_SIZE_CACHE
#define LARTC_TYPECHECK_SIZE_CACHE
#include <lartc/ast/declaration.hh>
#include <lartc/resolve/symbol_cache.hh>
#include <map>
#include <cstdint>

struct SizeCache {
  std::map<Declaration*, std::uintmax_t> sizes;
  std::map<Declaration*, bool> staging;

  static std::ostream& Print(std::ostream& out, SizeCache& size_cache);
  std::uintmax_t compute_size_of(SymbolCache& symbol_cache, Declaration* scope, Type* type);
  std::uintmax_t compute_size_in_byte_of(SymbolCache& symbol_cache, Declaration* scope, Type* type);
};
#endif//LARTC_TYPECHECK_SIZE_CACHE
