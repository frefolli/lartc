#ifndef LARTC_RESOLVE_SYMBOL_CACHE
#define LARTC_RESOLVE_SYMBOL_CACHE
#include <lartc/ast/declaration.hh>
#include <lartc/ast/statement.hh>
#include <lartc/ast/symbol.hh>
#include <lartc/resolve/symbol_stack.hh>
#include <map>

struct SymbolCache {
  std::map<Declaration*, std::map<Symbol, Declaration*>> globals;
  std::map<Expression*, Statement*> locals;
  std::map<Expression*, std::pair<std::string, Type*>*> parameters;

  Declaration* find_by_going_up(Declaration* context, Symbol& symbol, uint64_t progress = 0);
  Declaration* find_by_going_down(Declaration* context, Symbol& symbol, uint64_t progress = 0);

  Declaration* get_or_find_declaration(Declaration* context, Symbol& symbol);
  Declaration* get_declaration(Declaration* context, Symbol& symbol) const;
  
  Statement* get_or_find_statement(SymbolStack& symbol_stack, Expression* expression, Symbol& symbol);
  // Here context is the symbol
  Statement* get_statement(Expression* context) const;
  
  std::pair<std::string, Type*>* get_or_find_parameter(Declaration* context, Expression* expression, Symbol& symbol);
  // Here context is the symbol
  std::pair<std::string, Type*>* get_parameter(Expression* context) const;

  static std::ostream& Print(std::ostream& out, SymbolCache& symbol_cache);
};
#endif//LARTC_RESOLVE_SYMBOL_CACHE
