#include <cassert>
#include <iomanip>
#include <lartc/ast/expression.hh>
#include <lartc/internal_errors.hh>
#include <lartc/serializations.hh>

Expression* Expression::New(expression_t kind) {
  return new Expression {
    .kind = kind,
    .symbol = {},
    .string_literal = "",
    .boolean_literal = false,
    .integer_literal = 0,
    .decimal_literal = 0.0,
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
    expr->string_literal = "";
    expr->boolean_literal = false;
    expr->integer_literal = 0;
    expr->decimal_literal = 0.0;
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

std::ostream& Expression::Print(std::ostream& out, const Expression* expr, bool parenthesized) {
  assert(expr != nullptr);
  if (parenthesized && (expr->kind == expression_t::MONARY_EXPR
                        || expr->kind == expression_t::BINARY_EXPR))
    out << "(";
  bool first = true;
  switch (expr->kind) {
    case expression_t::SYMBOL_EXPR:
      Symbol::Print(out, expr->symbol);
      break;
    case expression_t::INTEGER_EXPR:
      out << expr->integer_literal;
      break;
    case expression_t::DOUBLE_EXPR:
      out << std::setprecision(19) << expr->decimal_literal;
      break;
    case expression_t::BOOLEAN_EXPR:
      out << dump_boolean(expr->boolean_literal);
      break;
    case expression_t::NULLPTR_EXPR:
      out << "nullptr";
      break;
    case expression_t::CHARACTER_EXPR:
      out << dump_unescaped_char(expr->integer_literal);
      break;
    case expression_t::STRING_EXPR:
      out << dump_unescaped_string(expr->string_literal);
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
      if (expr->operator_ != operator_t::DOT_OP && expr->operator_ != operator_t::ARR_OP) {
        out << " ";
      }
      out << expr->operator_;
      if (expr->operator_ != operator_t::DOT_OP && expr->operator_ != operator_t::ARR_OP) {
        out << " ";
      }
      Expression::Print(out, expr->right, parenthesized || expr->operator_ != operator_t::ASS_OP);
      break;
    case expression_t::ARRAY_ACCESS_EXPR:
      Expression::Print(out, expr->left, parenthesized);
      out << "[";
      Expression::Print(out, expr->right, parenthesized);
      out << "]";
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
    case expression_t::VANEXT_EXPR:
      out << "bitcast<";
      Type::Print(out, expr->type);
      out << ">";
      break;
   }
  if (parenthesized && (expr->kind == expression_t::MONARY_EXPR
                        || expr->kind == expression_t::BINARY_EXPR))
    out << ")";
  return out;
}
