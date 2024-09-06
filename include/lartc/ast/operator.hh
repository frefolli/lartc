#ifndef LARTC_AST_BINARY_OPERATOR
#define LARTC_AST_BINARY_OPERATOR
#include <ostream>
#define BINARY_OPERATOR_VARIANTS \
    X(DOT_OP), \
    X(ARR_OP), \
    X(XOR_OP), \
    X(MUL_OP), \
    X(DIV_OP), \
    X(ADD_OP), \
    X(SUB_OP), \
    X(GE_OP), \
    X(LE_OP), \
    X(SCA_OP), \
    X(SCO_OP), \
    X(EQ_OP), \
    X(NE_OP), \
    X(LROT_OP), \
    X(RROT_OP), \
    X(GR_OP), \
    X(LR_OP), \
    X(AND_OP), \
    X(OR_OP), \
    X(ASS_OP), \
    X(INC_OP), \
    X(DEC_OP), \
    X(TILDE_OP), \
    X(NOT_OP)

enum operator_t {
  #define X(_) _
    BINARY_OPERATOR_VARIANTS
  #undef X
};

void operator>>(std::string op, operator_t& operator_);
std::ostream& operator<<(std::ostream& out, operator_t operator_);
#endif//LARTC_AST_BINARY_OPERATOR
