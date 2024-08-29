#include <lartc/ast/declaration/variants.hh>

std::ostream& operator<<(std::ostream& out, declaration_t kind) {
  constexpr const char* _variants[] = {
    #define X(_) #_
      DECLARATION_VARIANTS
    #undef X
  };
  constexpr int _bound = (sizeof(_variants) / sizeof(const char *));

  // Int values can be coerced to declaration_t
  if (kind < _bound)
    return out << _variants[kind];
  return out << "<declaration-t>";
}
