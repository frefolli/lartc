#ifndef LARTC_AST_TYPE_VARIANTS
#define LARTC_AST_TYPE_VARIANTS
#include <ostream>
#define TYPE_VARIANTS \
  X(INTEGER_TYPE), \
  X(DOUBLE_TYPE), \
  X(BOOLEAN_TYPE), \
  X(POINTER_TYPE), \
  X(IDENTIFIER_TYPE), \
  X(VOID_TYPE), \
  X(STRUCT_TYPE), \
  X(FUNCTION_TYPE)
enum type_t {
  #define X(_) _
    TYPE_VARIANTS
  #undef X
};

std::ostream& operator<<(std::ostream& out, type_t kind);
#endif//LARTC_AST_TYPE_VARIANTS
