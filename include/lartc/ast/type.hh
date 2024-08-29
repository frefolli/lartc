#ifndef LARTC_AST_TYPE
#define LARTC_AST_TYPE
#include <lartc/ast/type/variants.hh>
#include <cstdint>
#include <vector>
#include <utility>

struct Type {
  type_t kind;
  uint64_t size;
  bool is_signed;
  Type* subtype;
  std::string identifier;
  std::vector<std::pair<std::string, Type*>> fields;
  std::vector<std::pair<std::string, Type*>> parameters;

  static Type* New(type_t kind);
  static std::ostream& Print(std::ostream&, Type* type, uint64_t tabulation = 0);
  static void Delete(Type*& type);
};
#endif//LARTC_AST_TYPE
