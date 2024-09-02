#include "lartc/ast/expression/variants.hh"
#include <lartc/ast/expression.hh>
#include <lartc/internal_errors.hh>

Expression* Expression::New(expression_t kind) {
  return new Expression {
    .kind = kind,
    .symbol = {},
    .literal = "",
    .callable = nullptr,
    .arguments = {},
    .operator_ = (operator_t)-1,
    .type = nullptr,
    .left = nullptr,
    .right = nullptr,
    .value = nullptr
  };
}

void Expression::Delete(Expression*& expr) {
  if (expr != nullptr) {
    expr->symbol = {};
    expr->literal.clear();
    Expression::Delete(expr->callable);
    for (Expression*& child : expr->arguments) {
      Expression::Delete(child);
    }
    expr->arguments.clear();
    expr->symbol = {};
    expr->operator_ = (operator_t)-1;
    Type::Delete(expr->type);
    delete expr;
    expr = nullptr;
  }
}

std::ostream& Expression::Print(std::ostream& out, Expression* expr, bool parenthesized) {
  if (parenthesized && (expr->kind == expression_t::MONARY_EXPR
                        || expr->kind == expression_t::BINARY_EXPR))
    out << "(";
  bool first = true;
  switch (expr->kind) {
    case expression_t::SYMBOL_EXPR:
      Symbol::Print(out, expr->symbol);
      break;
    case expression_t::INTEGER_EXPR:
      out << expr->literal;
      break;
    case expression_t::DOUBLE_EXPR:
      out << expr->literal;
      break;
    case expression_t::BOOLEAN_EXPR:
      out << expr->literal;
      break;
    case expression_t::NULLPTR_EXPR:
      out << "nullptr";
      break;
    case expression_t::CHARACTER_EXPR:
      out << expr->literal;
      break;
    case expression_t::STRING_EXPR:
      out << expr->literal;
      break;
    case expression_t::CALL_EXPR:
      Expression::Print(out, expr->callable) << "(";
      for (Expression* arg : expr->arguments) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        Expression::Print(out, arg);
      }
      out << ")";
      break;
    case expression_t::BINARY_EXPR:
      Expression::Print(out, expr->left, parenthesized || expr->operator_ != operator_t::ASS_OP);
      if (expr->operator_ != operator_t::DOT_OP) {
        out << " ";
      }
      out << expr->operator_;
      if (expr->operator_ != operator_t::DOT_OP) {
        out << " ";
      }
      Expression::Print(out, expr->right, parenthesized || expr->operator_ != operator_t::ASS_OP);
      break;
    case expression_t::MONARY_EXPR:
      out << expr->operator_;
      Expression::Print(out, expr->value, true);
      break;
    case expression_t::SIZEOF_EXPR:
      out << "sizeof<";
      Type::Print(out, expr->type);
      out << ">";
      break;
    case expression_t::CAST_EXPR:
      out << "cast<";
      Type::Print(out, expr->type);
      out << ">(";
      Expression::Print(out, expr->value);
      out << ")"; 
      break;
    case expression_t::BITCAST_EXPR:
      out << "bitcast<";
      Type::Print(out, expr->type);
      out << ">(";
      Expression::Print(out, expr->value);
      out << ")"; 
      break;
   }
  if (parenthesized && (expr->kind == expression_t::MONARY_EXPR
                        || expr->kind == expression_t::BINARY_EXPR))
    out << ")";
  return out;
}
