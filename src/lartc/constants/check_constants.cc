#include <cassert>
#include <iostream>
#include <lartc/ast/operator.hh>
#include <lartc/constants/check_constants.hh>
#include <lartc/external_errors.hh>
#include <lartc/internal_errors.hh>

std::pair<bool, std::pair<Expression*, Expression*>> cast_to_binexp_operands(Expression* left, Expression* right) {
  Expression* new_left = nullptr;
  Expression* new_right = nullptr;
  if (left->kind == expression_t::DOUBLE_EXPR || right->kind == expression_t::DOUBLE_EXPR) {
    new_left = Expression::New(DOUBLE_EXPR);
    new_right = Expression::New(DOUBLE_EXPR);
    if (left->kind == expression_t::DOUBLE_EXPR) {
      new_left->decimal_literal = left->decimal_literal;
    } else if (left->kind == expression_t::INTEGER_EXPR) {
      new_left->decimal_literal = left->integer_literal;
    } else if (left->kind == expression_t::BOOLEAN_EXPR) {
      new_left->decimal_literal = left->boolean_literal;
    } else if (left->kind == expression_t::CHARACTER_EXPR) {
      new_left->decimal_literal = left->integer_literal;
    }
    if (right->kind == expression_t::DOUBLE_EXPR) {
      new_right->decimal_literal = right->decimal_literal;
    } else if (right->kind == expression_t::INTEGER_EXPR) {
      new_right->decimal_literal = right->integer_literal;
    } else if (right->kind == expression_t::BOOLEAN_EXPR) {
      new_right->decimal_literal = right->boolean_literal;
    } else if (right->kind == expression_t::CHARACTER_EXPR) {
      new_right->decimal_literal = right->integer_literal;
    }
  } else if (left->kind == expression_t::INTEGER_EXPR || right->kind == expression_t::INTEGER_EXPR) {
    new_left = Expression::New(INTEGER_EXPR);
    new_right = Expression::New(INTEGER_EXPR);
    if (left->kind == expression_t::INTEGER_EXPR) {
      new_left->integer_literal = left->integer_literal;
    } else if (left->kind == expression_t::BOOLEAN_EXPR) {
      new_left->integer_literal = left->boolean_literal;
    } else if (left->kind == expression_t::CHARACTER_EXPR) {
      new_left->integer_literal = left->integer_literal;
    }
    if (right->kind == expression_t::INTEGER_EXPR) {
      new_right->integer_literal = right->integer_literal;
    } else if (right->kind == expression_t::BOOLEAN_EXPR) {
      new_right->integer_literal = right->boolean_literal;
    } else if (right->kind == expression_t::CHARACTER_EXPR) {
      new_right->integer_literal = right->integer_literal;
    }
  } else if (left->kind == expression_t::CHARACTER_EXPR || right->kind == expression_t::CHARACTER_EXPR) {
    new_left = Expression::New(INTEGER_EXPR);
    new_right = Expression::New(INTEGER_EXPR);
    if (left->kind == expression_t::BOOLEAN_EXPR) {
      new_left->integer_literal = left->boolean_literal;
    } else if (left->kind == expression_t::CHARACTER_EXPR) {
      new_left->integer_literal = left->integer_literal;
    }
    if (right->kind == expression_t::BOOLEAN_EXPR) {
      new_right->integer_literal = right->boolean_literal;
    } else if (right->kind == expression_t::CHARACTER_EXPR) {
      new_right->integer_literal = right->integer_literal;
    }
  } else if (left->kind == expression_t::BOOLEAN_EXPR || right->kind == expression_t::BOOLEAN_EXPR) {
    new_left = Expression::New(BOOLEAN_EXPR);
    new_right = Expression::New(BOOLEAN_EXPR);
    if (left->kind == expression_t::BOOLEAN_EXPR) {
      new_left->boolean_literal = left->boolean_literal;
    }
    if (right->kind == expression_t::BOOLEAN_EXPR) {
      new_right->boolean_literal = right->boolean_literal;
    }
  }
  bool ok = true;
  if (new_left == nullptr || new_right == nullptr) {
    ok = false;
    if (new_left != nullptr) {
      Expression::Delete(new_left);
    }
    if (new_right != nullptr) {
      Expression::Delete(new_right);
    }
  } 
  return {ok, {new_left, new_right}};
}

std::pair<bool, Expression*> cast_monexp_operand(Expression* expr) {
  Expression* result = nullptr;
  if (expr->kind == expression_t::INTEGER_EXPR) {
    result = Expression::Clone(expr);
  } else if (expr->kind == expression_t::DOUBLE_EXPR) {
    result = Expression::Clone(expr);
  } else if (expr->kind == expression_t::CHARACTER_EXPR) {
    result = Expression::New(expression_t::INTEGER_EXPR);
    result->integer_literal = expr->integer_literal;
  } else if (expr->kind == expression_t::BOOLEAN_EXPR) {
    result = Expression::Clone(expr);
  }
  bool ok = true;
  if (result == nullptr) {
    ok = false;
  }
  return {ok, result};
}

std::pair<bool, Expression*> check_constants(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, TypeCache& type_cache, ConstantCache& constant_cache, Declaration* decl, Expression* expr) {
  Expression* result = nullptr;
  bool ok = true;
  (void)file_db;
  (void)symbol_cache;
  (void)type_cache;
  (void)constant_cache;
  (void)decl;

  switch (expr->kind) {
    case VANEXT_EXPR:
      {
        ok = false;
        throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
        result = Expression::Clone(expr);
        break;
      }
    case CALL_EXPR:
      {
        ok = false;
        throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
        result = Expression::Clone(expr);
        break;
      }
    case ARRAY_ACCESS_EXPR:
      {
        ok = false;
        throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
        result = Expression::Clone(expr);
        break;
      }
    case INTEGER_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case DOUBLE_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case BOOLEAN_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case NULLPTR_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case CHARACTER_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case STRING_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case SIZEOF_EXPR:
      {
        result = Expression::New(INTEGER_EXPR);
        result->integer_literal = size_cache.compute_size_in_byte_of(symbol_cache, decl, expr->type);
        break;
      }
    case SYMBOL_EXPR:
      {
        Declaration* reference = symbol_cache.get_declaration(decl, expr->symbol);
        if (reference->kind == declaration_t::STATIC_VARIABLE_DECL) {
            if (constant_cache.staging[reference]) {
              throw_cyclic_dependency_between_static_variables_is_not_protected_by_usage_of_pointers(file_db, file_db.expression_points[expr], decl, reference);
              result = Expression::Clone(expr);
              ok = false;
            } else {
            if (!constant_cache.constants.contains(reference)) {
              constant_cache.staging[reference] = true;
              ok &= check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, reference);
              if (ok) {
                result = Expression::Clone(constant_cache.constants[reference]);
              } else {
                ok = false;
                result = Expression::Clone(expr);
              }
              constant_cache.staging[reference] = false;
            } else {
              result = Expression::Clone(constant_cache.constants[reference]);
            }
          }
        } else {
          ok = false;
          result = Expression::Clone(expr);
        }
        break;
      }
    case BINARY_EXPR:
      {
        auto left = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->left);
        auto right = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->right);
        ok &= left.first;
        ok &= right.first;
        if (ok) {
          auto casting = cast_to_binexp_operands(left.second, right.second);
          ok &= casting.first;
          auto operands = casting.second;
          if (ok) {
            switch (expr->operator_) {
              case XOR_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = (uintmax_t)(operands.first->decimal_literal) ^ (uintmax_t)(operands.second->decimal_literal);
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal ^ operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal ^ operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case MUL_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal * operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal * operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal & operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case DIV_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal / operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal / operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal / operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case MOD_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = (intmax_t)(operands.first->decimal_literal) % ((intmax_t)operands.second->decimal_literal);
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal % operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal % operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case ADD_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal + operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal + operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal + operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case SUB_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal - operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal - operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal - operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case GE_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal >= operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal >= operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal >= operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case LE_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal <= operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal <= operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal <= operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case SCA_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal && operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal && operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal && operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case SCO_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal || operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal || operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal || operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case EQ_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal == operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal == operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal == operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case NE_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal != operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal != operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal != operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case LROT_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = (uintmax_t)(operands.first->decimal_literal) << (uintmax_t)(operands.second->decimal_literal);
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal << operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = (uintmax_t)(operands.first->boolean_literal) << (uintmax_t)(operands.second->boolean_literal);
                  } else {
                    assert(false);
                  }
                  break;
                }
              case RROT_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = (uintmax_t)(operands.first->decimal_literal) >> (uintmax_t)(operands.second->decimal_literal);
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal >> operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = (uintmax_t)(operands.first->boolean_literal) >> (uintmax_t)(operands.second->boolean_literal);
                  } else {
                    assert(false);
                  }
                  break;
                }
              case GR_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal > operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal > operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal > operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case LR_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = operands.first->decimal_literal < operands.second->decimal_literal;
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal < operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal < operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case AND_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = (uintmax_t)(operands.first->decimal_literal) & (uintmax_t)(operands.second->decimal_literal);
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal & operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal & operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case OR_OP:
                {
                  result = Expression::Clone(operands.first);
                  if (operands.first->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = (uintmax_t)(operands.first->decimal_literal) | (uintmax_t)(operands.second->decimal_literal);
                  } else if (operands.first->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = operands.first->integer_literal | operands.second->integer_literal;
                  } else if (operands.first->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = operands.first->boolean_literal | operands.second->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              default:
                {
                  throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
                  ok = false;
                  result = Expression::Clone(expr);
                }
            }
            Expression::Delete(operands.first);
            Expression::Delete(operands.second);
          } else {
            throw_was_not_able_to_deduce_binexp_types(file_db, file_db.expression_points[expr], decl, expr->left, expr->right);
          }
        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
    case MONARY_EXPR:
      {
        auto value = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->value);
        ok &= value.first;
        if (ok) {
          auto casting = cast_monexp_operand(value.second);
          ok &= casting.first;
          auto operand = casting.second;
          if (ok) {
            switch (expr->operator_) {
              case ADD_OP:
                {
                  result = Expression::Clone(operand);
                  break;
                }
              case SUB_OP:
                {
                  result = Expression::Clone(operand);
                  if (operand->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = -operand->decimal_literal;
                  } else if (operand->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = -operand->integer_literal;
                  } else if (operand->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = -operand->boolean_literal;
                  } else {
                    assert(false);
                  }
                  break;
                }
              case NOT_OP:
                {
                  result = Expression::Clone(operand);
                  if (operand->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = !(uintmax_t)(operand->decimal_literal);
                  } else if (operand->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = !(operand->integer_literal);
                  } else if (operand->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = !(operand->boolean_literal);
                  } else {
                    assert(false);
                  }
                  break;
                }
              case TILDE_OP:
                {
                  result = Expression::Clone(operand);
                  if (operand->kind == expression_t::DOUBLE_EXPR) {
                    result->decimal_literal = ~(uintmax_t)(operand->decimal_literal);
                  } else if (operand->kind == expression_t::INTEGER_EXPR) {
                    result->integer_literal = ~(operand->integer_literal);
                  } else if (operand->kind == expression_t::BOOLEAN_EXPR) {
                    result->boolean_literal = !(operand->boolean_literal);
                  } else {
                    assert(false);
                  }
                  break;
                }
              default:
                {
                  throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
                  ok = false;
                  result = Expression::Clone(expr);
                }
            }
            Expression::Delete(operand);
          } else {
            throw_was_not_able_to_deduce_monexp_types(file_db, file_db.expression_points[expr], decl, value.second);
          }
        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
    case CAST_EXPR:
      {
        auto checked = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->value);
        ok &= checked.first;
        if (ok) {
          // to trigger not implemented
          result = nullptr;
        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
    case BITCAST_EXPR:
      {
        auto checked = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->value);
        ok &= checked.first;
        if (ok) {
          // to trigger not implemented
          result = nullptr;
        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
  }
  if (result == nullptr) {
    throw_constant_expression_is_not_implemented(file_db, file_db.expression_points[expr], decl);
    result = Expression::Clone(expr);
    ok = false;
  }
  assert(result != nullptr);
  return {ok, result};
}

bool check_constants(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, TypeCache& type_cache, ConstantCache& constant_cache, Declaration* decl) {
  bool declared_types_ok = true;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      for (auto child : decl->children) {
        declared_types_ok &= check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, child);
      }
      break;
    case declaration_t::STATIC_VARIABLE_DECL:
      if (!constant_cache.constants.contains(decl)) {
        constant_cache.staging[decl] = true;
        if (decl->value != nullptr) {
          auto checked = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, decl->value);
          constant_cache.constants[decl] = checked.second;
          declared_types_ok &= checked.first;
        }
        constant_cache.staging[decl] = false;
      }
      break;
    case declaration_t::FUNCTION_DECL:
      break;
    case declaration_t::TYPE_DECL:
      break;
  }
  return declared_types_ok;
}
