#ifndef LARTC_RESOLVE_SYMBOL_STACK
#define LARTC_RESOLVE_SYMBOL_STACK
#include <lartc/ast/symbol.hh>
#include <lartc/ast/type.hh>
#include <string>
#include <vector>
#include <map>

struct SymbolStack {
  std::vector<std::map<std::string, Type*>> stack;

  void open_scope();
  void close_scope();
  bool set(std::string, Type*);
  Type* get(Symbol& symbol);
};
#endif//LARTC_RESOLVE_SYMBOL_STACK
