#include <lartc/ast/statement/variants.hh>

std::ostream& operator<<(std::ostream& out, statement_t kind) {
  constexpr const char* _variants[] = {
    #define X(_) #_
      STATEMENT_VARIANTS
    #undef X
  };
  constexpr int _bound = (sizeof(_variants) / sizeof(const char *));

  // Int values can be coerced to statement_t
  if (kind < _bound)
    return out << _variants[kind];
  return out << "<statement-t>";
}
