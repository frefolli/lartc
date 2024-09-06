#ifndef LARTC_AST_TYPE
#define LARTC_AST_TYPE
#include <lartc/ast/type/variants.hh>
#include <lartc/ast/symbol.hh>
#include <cstdint>
#include <vector>
#include <utility>

struct Type {
  type_t kind;
  uint64_t size;
  bool is_signed;
  Type* subtype;
  Symbol symbol;
  std::vector<std::pair<std::string, Type*>> fields;
  std::vector<std::pair<std::string, Type*>> parameters;

  static Type* New(type_t kind);
  static Type* Clone(Type* other);
  static std::ostream& Print(std::ostream&, Type* type, uint64_t tabulation = 0);
  static void Delete(Type*& type);
  static Type* ExtractField(Type* struct_type, Symbol& name);
};
#endif//LARTC_AST_TYPE
