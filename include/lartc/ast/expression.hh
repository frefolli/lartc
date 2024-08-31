#ifndef LARTC_AST_EXPRESSION
#define LARTC_AST_EXPRESSION
#include <lartc/ast/expression/variants.hh>
#include <lartc/ast/type.hh>
#include <cstdint>
#include <vector>

struct Expression {
  expression_t kind;
  std::string name; // and scoped name for now
  std::string literal;
  Expression* callable;
  std::vector<Expression*> arguments;
  std::string operator_;
  Type* type;
  Expression* left;
  Expression* right;
  Expression* value;

  static Expression* New(expression_t kind);
  static std::ostream& Print(std::ostream& out, Expression* decl, uint64_t tabulation = 0);
  static void Delete(Expression*& decl);
};
#endif//LARTC_AST_EXPRESSION
