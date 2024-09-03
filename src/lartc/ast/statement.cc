#include "lartc/ast/expression.hh"
#include <lartc/ast/statement.hh>
#include <lartc/internal_errors.hh>

Statement* Statement::New(statement_t kind) {
  return new Statement {
    .kind = kind,
    .children = {},
    .name = "",
    .type = nullptr,
    .condition = nullptr,
    .then = nullptr,
    .else_ = nullptr,
    .init = nullptr,
    .step = nullptr,
    .body = nullptr,
    .expr = nullptr
  };
}

void Statement::Delete(Statement*& stmt) {
  if (stmt != nullptr) {
    for (Statement*& child : stmt->children) {
      Statement::Delete(child);
    }
    stmt->children = {};
    stmt->name = "";
    Type::Delete(stmt->type);
    Expression::Delete(stmt->condition);
    Statement::Delete(stmt->then);
    Statement::Delete(stmt->else_);
    Statement::Delete(stmt->init);
    Expression::Delete(stmt->step);
    Statement::Delete(stmt->body);
    Expression::Delete(stmt->expr);
    delete stmt;
    stmt = nullptr;
  }
}

std::ostream& Statement::Print(std::ostream& out, Statement* stmt, uint64_t tabulation) {
  if (stmt->kind != statement_t::BLOCK_STMT)
    tabulate(out, tabulation);
  bool first = true;
  switch (stmt->kind) {
    case statement_t::FOR_STMT:
      out << "for (";
      Statement::Print(out, stmt->init);
      Expression::Print(out << " ", stmt->condition);
      out << "; ";
      Expression::Print(out, stmt->step);
      out << ") ";
      Statement::Print(out, stmt->body, tabulation);
      break;
    case statement_t::LET_STMT:
      out << "let " << stmt->name << ": ";
      Type::Print(out, stmt->type);
      if (stmt->expr != nullptr) {
        Expression::Print(out << " = ", stmt->expr);
      }
      out << ";";
      break;
    case statement_t::BLOCK_STMT:
      out << "{" << std::endl;
      for (Statement* child : stmt->children) {
        if (first) {
          first = false;
        } else {
          out << std::endl;
        }
        Statement::Print(out, child, tabulation + 1);
      }
      tabulate(out<< std::endl, tabulation) << "}";
      break;
    case statement_t::BREAK_STMT:
      out << "break;";
      break;
    case statement_t::RETURN_STMT:
      out << "return";
      if (stmt->expr != nullptr) {
        Expression::Print(out << " ", stmt->expr);
      }
      out << ";";
      break;
    case statement_t::CONTINUE_STMT:
      out << "continue;";
      break;
    case statement_t::IF_ELSE_STMT:
      out << "if (";
      Expression::Print(out, stmt->condition);
      out << ") ";
      Statement::Print(out, stmt->then, tabulation);
      tabulate(out << std::endl, tabulation) << "else ";
      Statement::Print(out, stmt->then, tabulation);
      break;
    case statement_t::WHILE_STMT:
      out << "while (";
      Expression::Print(out, stmt->condition);
      out << ") ";
      Statement::Print(out, stmt->body, tabulation);
      break;
    case statement_t::EXPRESSION_STMT:
      Expression::Print(out, stmt->expr) << ";";
      break;
  }
  return out;
}

std::ostream& Statement::PrintShort(std::ostream& out, Statement* stmt) {
  switch (stmt->kind) {
    case statement_t::FOR_STMT:
      break;
    case statement_t::LET_STMT:
      out << "let " << stmt->name << ": ";
      Type::Print(out, stmt->type) << ";";
      break;
    case statement_t::BLOCK_STMT:
      break;
    case statement_t::BREAK_STMT:
      break;
    case statement_t::RETURN_STMT:
      break;
    case statement_t::CONTINUE_STMT:
      break;
    case statement_t::IF_ELSE_STMT:
      break;
    case statement_t::WHILE_STMT:
      break;
    case statement_t::EXPRESSION_STMT:
      break;
  }
  return out;
}
