#ifndef LARTC_AST_DECLARATION
#define LARTC_AST_DECLARATION
#include <lartc/ast/declaration/variants.hh>
#include <lartc/ast/type.hh>
#include <vector>
#include <cstdint>

struct Declaration {
  declaration_t kind;
  std::vector<Declaration*> children;
  std::string name;
  Declaration* parent;
  Type* type;

  static Declaration* New(declaration_t kind);
  static std::ostream& Print(std::ostream& out, Declaration* decl, uint64_t tabulation = 0);
  static void Delete(Declaration*& decl);
};
#endif//LARTC_AST_DECLARATION
