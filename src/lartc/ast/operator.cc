#include <lartc/ast/operator.hh>
#include <cassert>

void operator>>(std::string op, operator_t& operator_) {
  if (op == ".")
    operator_ = DOT_OP;
  if (op == "->")
    operator_ = ARR_OP;
  if (op == "^")
    operator_ = XOR_OP;
  if (op == "*")
    operator_ = MUL_OP;
  if (op == "/")
    operator_ = DIV_OP;
  if (op == "%")
    operator_ = MOD_OP;
  if (op == "+")
    operator_ = ADD_OP;
  if (op == "-")
    operator_ = SUB_OP;
  if (op == ">=")
    operator_ = GE_OP;
  if (op == "<=")
    operator_ = LE_OP;
  if (op == "&&")
    operator_ = SCA_OP;
  if (op == "||")
    operator_ = SCO_OP;
  if (op == "==")
    operator_ = EQ_OP;
  if (op == "!=")
    operator_ = NE_OP;
  if (op == "<<")
    operator_ = LROT_OP;
  if (op == ">>")
    operator_ = RROT_OP;
  if (op == ">")
    operator_ = GR_OP;
  if (op == "<")
    operator_ = LR_OP;
  if (op == "&")
    operator_ = AND_OP;
  if (op == "|")
    operator_ = OR_OP;
  if (op == "=")
    operator_ = ASS_OP;
  if (op == "++")
    operator_ = INC_OP;
  if (op == "--")
    operator_ = DEC_OP;
  if (op == "~")
    operator_ = TILDE_OP;
  if (op == "!")
    operator_ = NOT_OP;
}

std::ostream& operator<<(std::ostream& out, operator_t operator_) {
  switch (operator_) {
    case DOT_OP: return out << ".";
    case ARR_OP: return out << "->";
    case XOR_OP: return out << "^";
    case MUL_OP: return out << "*";
    case DIV_OP: return out << "/";
    case MOD_OP: return out << "%";
    case ADD_OP: return out << "+";
    case SUB_OP: return out << "-";
    case GE_OP: return out << ">=";
    case LE_OP: return out << "<=";
    case SCA_OP: return out << "&&";
    case SCO_OP: return out << "||";
    case EQ_OP: return out << "==";
    case NE_OP: return out << "!=";
    case LROT_OP: return out << "<<";
    case RROT_OP: return out << ">>";
    case GR_OP: return out << ">";
    case LR_OP: return out << "<";
    case AND_OP: return out << "&";
    case OR_OP: return out << "|";
    case ASS_OP: return out << "=";
    case INC_OP: return out << "++";
    case DEC_OP: return out << "--";
    case TILDE_OP: return out << "~";
    case NOT_OP: return out << "!";
  }
  return out << "operator-t>";
}

operator_category_t get_operator_category(operator_t operator_) {
  switch (operator_) {
    case DOT_OP:
      return FIELD_ACCESS_OPC;
    case ARR_OP:
      return POINTED_FIELD_ACCESS_OPC;
    case NOT_OP:
    case EQ_OP:
    case NE_OP:
    case SCA_OP:
    case SCO_OP:
    case AND_OP:
    case OR_OP:
    case GE_OP:
    case LE_OP:
    case GR_OP:
    case LR_OP:
      return LOGICAL_OPC;
    case XOR_OP:
    case TILDE_OP:
    case MUL_OP:
    case DIV_OP:
    case MOD_OP:
    case ADD_OP:
    case SUB_OP:
    case INC_OP:
    case DEC_OP:
    case LROT_OP:
    case RROT_OP:
      return ALGEBRAIC_OPC;
    case ASS_OP:
      return ASSIGNMENT_OPC;
  }

  assert(nullptr);
}
