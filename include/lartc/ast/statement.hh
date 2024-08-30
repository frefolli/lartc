#ifndef LARTC_AST_STATEMENT
#define LARTC_AST_STATEMENT
#include <lartc/ast/statement/variants.hh>
#include <lartc/ast/type.hh>
#include <cstdint>

struct Statement {
  statement_t kind;

  static Statement* New(statement_t kind);
  static std::ostream& Print(std::ostream& out, Statement* decl, uint64_t tabulation = 0);
  static void Delete(Statement*& decl);
};
#endif//LARTC_AST_STATEMENT
