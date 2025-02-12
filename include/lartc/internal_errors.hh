#ifndef LARTC_INTERNAL_ERROR
#define LARTC_INTERNAL_ERROR
#include <sstream>
#include <cstdint>
#include <lartc/ast/declaration.hh>
#include <lartc/ast/symbol.hh>

#define MSG(...) \
  std::stringstream() << __VA_ARGS__

#define INTERNAL_ERROR_VARIANTS \
  X(NO_ERROR), \
  X(UNHANDLED_TS_SYMBOL_NAME), \
  X(TS_ROOT_NODE_SHOULD_BE_SOURCE_FILE), \
  X(TS_UNEXPECTED_SYMBOL_NAME), \
  X(TS_DEBUG), \
  X(SYMBOL_SHOULD_BE_RESOLVED), \
  X(NOT_IMPLEMENTED), \
  X(ATTEMPT_TO_CLONE_NULLPTR_AS_TYPE), \
  X(ATTEMPT_TO_USE_OTHER_DECLARATION_AS_TYPEDEF), \
  X(INCLUDE_DIRECTORY_SHOULD_HAVE_LOCAL_OR_GLOBAL_PATH)

enum internal_error_t {
#define X(_) _
  INTERNAL_ERROR_VARIANTS
#undef X
};

std::ostream& operator<<(std::ostream& out, internal_error_t error);
void throw_internal_error(internal_error_t error, std::stringstream msg);
void throw_internal_error(internal_error_t error, std::string msg);

inline std::ostream& tabulate(std::ostream& out, std::uintmax_t tabulation) {
  for (std::uintmax_t i = 0; i < tabulation; ++i) {
    out << "  ";
  }
  return out;
}

void throw_symbol_should_be_resolved(Declaration* context, Symbol& symbol);
void throw_attempt_to_use_other_declaration_as_typedef(Declaration* context, Declaration* decl);
#endif//LARTC_INTERNAL_ERROR
