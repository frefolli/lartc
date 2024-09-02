#include <lartc/ast/symbol.hh>
#include <cstring>

Symbol Symbol::From(std::string name) {
  Symbol symbol = {.identifiers = {}};
  char* token = std::strtok(name.data(), "::");
  while (token != nullptr) {
    symbol.identifiers.push_back(token);
    token = std::strtok(nullptr, "::");
  }
  return symbol;
}

std::ostream& Symbol::Print(std::ostream& out, Symbol& symbol) {
  bool first = true;
  for (std::string& identifier : symbol.identifiers) {
    if (first) {
      first = false;
    } else {
      out << "::";
    }
    out << identifier;
  }
  return out;
}
