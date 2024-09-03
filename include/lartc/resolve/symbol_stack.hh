#ifndef LARTC_RESOLVE_SYMBOL_STACK
#define LARTC_RESOLVE_SYMBOL_STACK
#include <lartc/ast/symbol.hh>
#include <lartc/ast/statement.hh>
#include <string>
#include <vector>
#include <map>

struct SymbolStack {
  std::vector<std::map<std::string, Statement*>> stack;

  void open_scope();
  void close_scope();
  bool set(std::string, Statement*);
  Statement* get(Symbol& symbol);

  static std::ostream& Print(std::ostream& out, SymbolStack& symbol_stack);
};
#endif//LARTC_RESOLVE_SYMBOL_STACK
