#include "lartc/ast/statement/variants.hh"
#include <lartc/ast/statement.hh>
#include <lartc/internal_errors.hh>

Statement* Statement::New(statement_t kind) {
  return new Statement {kind};
}

void Statement::Delete(Statement*& stmt) {
  if (stmt != nullptr) {
    delete stmt;
    stmt = nullptr;
  }
}

std::ostream& Statement::Print(std::ostream& out, Statement* stmt, uint64_t tabulation) {
  tabulate(out, tabulation);
  switch (stmt->kind) {
    case statement_t::FOR_STMT:
      return out << "for";
    case statement_t::LET_STMT:
      return out << "let";
    case statement_t::BLOCK_STMT:
      return out << "{}";
    case statement_t::BREAK_STMT:
      return out << "break";
    case statement_t::CONST_STMT:
      return out << "const";
    case statement_t::RETURN_STMT:
      return out << "return";
    case statement_t::CONTINUE_STMT:
      return out << "continue";
    case statement_t::IF_ELSE_STMT:
      return out << "if-else";
    case statement_t::WHILE_STMT:
      return out << "while";
  }
  return out;
}
