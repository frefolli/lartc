#ifndef LARTC_AST_STATEMENT
#define LARTC_AST_STATEMENT
#include <lartc/ast/statement/variants.hh>
#include <lartc/ast/type.hh>
#include <lartc/ast/expression.hh>
#include <cstdint>
#include <vector>

struct Statement {
  statement_t kind;
  std::vector<Statement*> children;
  std::string name;
  Type* type;
  Expression* condition;
  Statement* then;
  Statement* else_;
  Statement* init;
  Expression* step;
  Statement* body;
  Expression* expr;

  static Statement* New(statement_t kind);
  static std::ostream& Print(std::ostream& out, const Statement* decl, uint64_t tabulation = 0);
  static std::ostream& PrintShort(std::ostream& out, const Statement* decl);
  static void Delete(Statement*& decl);
};
#endif//LARTC_AST_STATEMENT
