#ifndef LARTC_AST_SYMBOL
#define LARTC_AST_SYMBOL
#include <string>
#include <vector>
#include <ostream>
struct Symbol {
  std::vector<std::string> identifiers;

  bool operator<(const Symbol& other) const;

  static Symbol From(std::string);
  static std::ostream& Print(std::ostream& out, const Symbol& symbol);
};
#endif//LARTC_AST_SYMBOL
