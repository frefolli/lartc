#ifndef LARTC_AST_DECLARATION
#define LARTC_AST_DECLARATION
#include <lartc/ast/declaration/variants.hh>
#include <lartc/ast/type.hh>
#include <lartc/ast/statement.hh>
#include <vector>
#include <cstdint>

struct Declaration {
  declaration_t kind;
  std::vector<Declaration*> children;
  std::string name;
  Declaration* parent;
  Type* type;
  std::vector<std::pair<std::string, Type*>> parameters;
  Statement* body;

  static Declaration* New(declaration_t kind);
  static std::ostream& Print(std::ostream& out, const Declaration* decl, uint64_t tabulation = 0);
  static void Delete(Declaration*& decl);
  static std::string QualifiedName(const Declaration* decl);
  static std::ostream& PrintShort(std::ostream& out, const Declaration* decl);
  Declaration* find_child(const std::string& name) const;
  void remove_child(const Declaration* target);
};
#endif//LARTC_AST_DECLARATION
