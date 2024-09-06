#include <lartc/ast/symbol.hh>
#include <cstring>
#include <cstdint>

Symbol Symbol::From(std::string name) {
  Symbol symbol = {.identifiers = {}};
  char* token = std::strtok(name.data(), "::");
  while (token != nullptr) {
    symbol.identifiers.push_back(token);
    token = std::strtok(nullptr, "::");
  }
  return symbol;
}

std::ostream& Symbol::Print(std::ostream& out, const Symbol& symbol) {
  bool first = true;
  for (const std::string& identifier : symbol.identifiers) {
    if (first) {
      first = false;
    } else {
      out << "::";
    }
    out << identifier;
  }
  return out;
}

bool Symbol::operator<(const Symbol& other) const {
  uint64_t i = 0, j = 0;
  while (i < identifiers.size() && j < other.identifiers.size()) {
    if (identifiers.at(i) < other.identifiers.at(j)) {
      return true;
    } else if (identifiers.at(i) > other.identifiers.at(j)) {
      return false;
    } else {
      i += 1;
      j += 1;
    }
  }
  return i < j;
}

bool Symbol::operator==(const Symbol& other) const {
  return identifiers == other.identifiers;
}
