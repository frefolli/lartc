#include <iomanip>
#include <lartc/codegen/literal_store.hh>
#include <sstream>

std::string LiteralStore::get_string_literal(const std::string& literal) {
  std::uintmax_t marker;
  if (string_literals.contains(literal)) {
    marker = string_literals[literal];
  } else {
    marker = count++;
    string_literals[literal] = marker;
  }
  return serialize(marker);
}

std::string LiteralStore::get_int_literal(std::intmax_t literal) {
  return std::to_string(literal);
}

std::string LiteralStore::get_float_literal(double_t literal) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(19);
  out << literal;
  return out.str();
}
