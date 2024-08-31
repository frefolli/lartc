#include <lartc/ast/expression.hh>
#include <lartc/internal_errors.hh>

Expression* Expression::New(expression_t kind) {
  return new Expression {
    .kind = kind,
    .name = "",
    .literal = "",
    .callable = nullptr,
    .arguments = {},
    .operator_ = "",
    .type = nullptr,
    .left = nullptr,
    .right = nullptr,
    .value = nullptr
  };
}

void Expression::Delete(Expression*& expr) {
  if (expr != nullptr) {
    expr->name.clear();
    expr->literal.clear();
    Expression::Delete(expr->callable);
    for (Expression*& child : expr->arguments) {
      Expression::Delete(child);
    }
    expr->arguments.clear();
    expr->name.clear();
    expr->operator_.clear();
    Type::Delete(expr->type);
    delete expr;
    expr = nullptr;
  }
}

std::ostream& Expression::Print(std::ostream& out, Expression* expr, uint64_t tabulation) {
  bool first = true;
  switch (expr->kind) {
    case expression_t::IDENTIFIER_EXPR:
      out << expr->name;
      break;
    case expression_t::SCOPED_IDENTIFIER_EXPR:
      out << expr->name;
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
      Expression::Print(out, expr->left);
      out << " " << expr->operator_ << " ";
      Expression::Print(out, expr->right);
      break;
    case expression_t::MONARY_EXPR:
      out << " " << expr->operator_ << " ";
      Expression::Print(out, expr->value);
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
  return out;
}
