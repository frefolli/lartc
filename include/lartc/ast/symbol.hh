#ifndef LARTC_AST_SYMBOL
#define LARTC_AST_SYMBOL
#include <string>
#include <vector>
#include <ostream>
struct Symbol {
  std::vector<std::string> identifiers;

  static Symbol From(std::string);
  static std::ostream& Print(std::ostream& out, Symbol& symbol);
};
#endif//LARTC_AST_SYMBOL
