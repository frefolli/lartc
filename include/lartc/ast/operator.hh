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

#define BINARY_OPERATOR_CATEGORY_VARIANTS \
  X(FIELD_ACCESS_OPC), \
  X(POINTED_FIELD_ACCESS_OPC), \
  X(ALGEBRAIC_OPC), \
  X(LOGICAL_OPC), \
  X(ASSIGNMENT_OPC)

enum operator_category_t {
  #define X(_) _
    BINARY_OPERATOR_CATEGORY_VARIANTS
  #undef X
};

void operator>>(std::string op, operator_t& operator_);
std::ostream& operator<<(std::ostream& out, operator_t operator_);
operator_category_t get_operator_category(operator_t operator_);
inline bool is_algebraic_operator(operator_t operator_) {
  return get_operator_category(operator_) == ALGEBRAIC_OPC;
}
inline bool is_logical_operator(operator_t operator_) {
  return get_operator_category(operator_) == LOGICAL_OPC;
}
#endif//LARTC_AST_BINARY_OPERATOR
