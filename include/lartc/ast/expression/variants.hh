#ifndef LARTC_AST_EXPRESSION_VARIANTS
#define LARTC_AST_EXPRESSION_VARIANTS
#include <ostream>

#define EXPRESSION_VARIANTS \
  X(IDENTIFIER_EXPR), \
  X(SCOPED_IDENTIFIER_EXPR), \
  X(INTEGER_EXPR), \
  X(DOUBLE_EXPR), \
  X(BOOLEAN_EXPR), \
  X(NULLPTR_EXPR), \
  X(CHARACTER_EXPR), \
  X(STRING_EXPR), \
  X(CALL_EXPR), \
  X(BINARY_EXPR), \
  X(MONARY_EXPR), \
  X(SIZEOF_EXPR), \
  X(CAST_EXPR), \
  X(BITCAST_EXPR)
  
enum expression_t {
#define X(_) _
  EXPRESSION_VARIANTS
#undef X
};

std::ostream& operator<<(std::ostream& out, expression_t kind);
#endif//LARTC_AST_EXPRESSION_VARIANTS
