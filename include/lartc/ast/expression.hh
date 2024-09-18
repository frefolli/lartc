#ifndef LARTC_AST_EXPRESSION
#define LARTC_AST_EXPRESSION
#include <lartc/ast/expression/variants.hh>
#include <lartc/ast/symbol.hh>
#include <lartc/ast/type.hh>
#include <lartc/ast/operator.hh>
#include <vector>

struct Expression {
  expression_t kind;
  Symbol symbol;
  std::string literal;
  Expression* callable;
  std::vector<Expression*> arguments;
  operator_t operator_;
  Type* type;
  Expression* left;
  Expression* right;
  Expression* value;

  static Expression* New(expression_t kind);
  static std::ostream& Print(std::ostream& out, const Expression* decl, bool parenthesized = false);
  static void Delete(Expression*& decl);
};
#endif//LARTC_AST_EXPRESSION
