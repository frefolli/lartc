#include <lartc/codegen/literal_store.hh>

std::string LiteralStore::get_string_literal(const std::string& literal) {
  uint64_t marker;
  if (string_literals.contains(literal)) {
    marker = string_literals[literal];
  } else {
    marker = count++;
    string_literals[literal] = marker;
  }
  return serialize(marker);
}

std::string LiteralStore::get_int_literal(int64_t literal) {
  uint64_t marker;
  if (int_literals.contains(literal)) {
    marker = int_literals[literal];
  } else {
    marker = count++;
    int_literals[literal] = marker;
  }
  return serialize(marker);
}

std::string LiteralStore::get_float_literal(double_t literal) {
  uint64_t marker;
  if (float_literals.contains(literal)) {
    marker = float_literals[literal];
  } else {
    marker = count++;
    float_literals[literal] = marker;
  }
  return serialize(marker);
}
