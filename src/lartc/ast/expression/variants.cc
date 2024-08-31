#include <lartc/ast/expression/variants.hh>

std::ostream& operator<<(std::ostream& out, expression_t kind) {
  constexpr const char* _variants[] = {
    #define X(_) #_
      EXPRESSION_VARIANTS
    #undef X
  };
  constexpr int _bound = (sizeof(_variants) / sizeof(const char *));

  // Int values can be coerced to expression_t
  if (kind < _bound)
    return out << _variants[kind];
  return out << "<expression-t>";
}
