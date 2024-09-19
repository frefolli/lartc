#ifndef LARTC__CODEGEN__MARKERS
#define LARTC__CODEGEN__MARKERS
#include <cstdint>
#include <string>

struct Markers {
  uint64_t count = 0;

  std::string last_marker() {
    return "%" + std::to_string(count);
  }

  std::string new_marker() {
    ++count;
    return "%" + std::to_string(count);
  }
};
#endif//LARTC__CODEGEN__MARKERS
