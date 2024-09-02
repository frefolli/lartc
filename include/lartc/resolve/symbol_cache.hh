#ifndef LARTC_RESOLVE_SYMBOL_CACHE
#define LARTC_RESOLVE_SYMBOL_CACHE
#include <lartc/ast/declaration.hh>
#include <lartc/ast/symbol.hh>
#include <map>

struct SymbolCache {
  std::map<Declaration*, std::map<Symbol, Declaration*>> cache;

  Declaration* find_by_going_up(Declaration* context, Symbol& symbol, uint64_t progress = 0);
  Declaration* find_by_going_down(Declaration* context, Symbol& symbol, uint64_t progress = 0);
  Declaration* find(Declaration* context, Symbol symbol);

  static std::ostream& Print(std::ostream& out, SymbolCache& symbol_cache);
};
#endif//LARTC_RESOLVE_SYMBOL_CACHE
