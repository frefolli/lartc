#include <lartc/ast/type/variants.hh>

std::ostream& operator<<(std::ostream& out, type_t kind) {
  constexpr const char* _variants[] = {
    #define X(_) #_
      TYPE_VARIANTS
    #undef X
  };
  constexpr int _bound = (sizeof(_variants) / sizeof(const char *));

  // Int values can be coerced to type_t
  if (kind < _bound)
    return out << _variants[kind];
  return out << "<type-t>";
}
