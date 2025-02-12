#ifndef LARTC__CODEGEN__LITERAL_STORE
#define LARTC__CODEGEN__LITERAL_STORE
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cmath>

struct LiteralStore {
  std::unordered_map<std::string, std::uintmax_t> string_literals;
  std::unordered_map<std::intmax_t, std::uintmax_t> int_literals;
  std::unordered_map<double_t, std::uintmax_t> float_literals;
  std::uintmax_t count = 1;

  inline std::string serialize(std::uintmax_t marker) {
    return "@.l" + std::to_string(marker);
  }

  std::string get_string_literal(const std::string& literal);
  std::string get_int_literal(std::intmax_t literal);
  std::string get_float_literal(double_t literal);
};
#endif//LARTC__CODEGEN__LITERAL_STORE
