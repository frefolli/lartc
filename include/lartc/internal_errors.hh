#ifndef LARTC_INTERNAL_ERROR
#define LARTC_INTERNAL_ERROR
#include <sstream>
#include <cstdint>

#define MSG(...) \
  std::stringstream() << __VA_ARGS__

#define INTERNAL_ERROR_VARIANTS \
  X(NO_ERROR), \
  X(UNHANDLED_TS_SYMBOL_NAME), \
  X(TS_ROOT_NODE_SHOULD_BE_SOURCE_FILE), \
  X(TS_UNEXPECTED_SYMBOL_NAME), \
  X(TS_DEBUG), \
  X(SYMBOL_SHOULD_BE_RESOLVE), \
  X(NOT_IMPLEMENTED), \
  X(ATTEMPT_TO_CLONE_NULLPTR_AS_TYPE)

enum internal_error_t {
#define X(_) _
  INTERNAL_ERROR_VARIANTS
#undef X
};

std::ostream& operator<<(std::ostream& out, internal_error_t error);
void throw_internal_error(internal_error_t error, std::stringstream msg);

inline std::ostream& tabulate(std::ostream& out, uint64_t tabulation) {
  for (uint64_t i = 0; i < tabulation; ++i) {
    out << "  ";
  }
  return out;
}
#endif//LARTC_INTERNAL_ERROR
