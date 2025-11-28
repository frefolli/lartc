#ifndef LARTC_AST_EXPRESSION
#define LARTC_AST_EXPRESSION
#include <lartc/ast/expression/variants.hh>
#include <lartc/ast/symbol.hh>
#include <lartc/ast/type.hh>
#include <lartc/ast/operator.hh>
#include <cmath>
#include <vector>

struct Expression {
  expression_t kind;
  Symbol symbol;
  std::string string_literal;
  bool boolean_literal;
  std::intmax_t integer_literal;
  double_t decimal_literal;
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
  static Expression* Clone(Expression* expr);
};
#endif//LARTC_AST_EXPRESSION
