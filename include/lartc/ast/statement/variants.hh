#ifndef LARTC_AST_STATEMENT_VARIANTS
#define LARTC_AST_STATEMENT_VARIANTS
#include <ostream>

#define STATEMENT_VARIANTS \
      X(LET_STMT), \
      X(RETURN_STMT), \
      X(BREAK_STMT), \
      X(CONTINUE_STMT), \
      X(IF_ELSE_STMT), \
      X(WHILE_STMT), \
      X(FOR_STMT), \
      X(BLOCK_STMT), \
      X(EXPRESSION_STMT)

enum statement_t {
#define X(_) _
  STATEMENT_VARIANTS
#undef X
};

std::ostream& operator<<(std::ostream& out, statement_t kind);
#endif//LARTC_AST_STATEMENT_VARIANTS
