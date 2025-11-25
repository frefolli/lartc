#ifndef LARTC_AST_DECLARATION_VARIANTS
#define LARTC_AST_DECLARATION_VARIANTS
#include <ostream>

#define DECLARATION_VARIANTS \
  X(MODULE_DECL), \
  X(FUNCTION_DECL), \
  X(STATIC_VARIABLE_DECL), \
  X(TYPE_DECL)
enum declaration_t {
#define X(_) _
  DECLARATION_VARIANTS
#undef X
};

std::ostream& operator<<(std::ostream& out, declaration_t kind);
#endif//LARTC_AST_DECLARATION_VARIANTS
