#include <lartc/typecheck/check_types.hh>
#include <lartc/typecheck/casting.hh>
#include <lartc/internal_errors.hh>
#include <lartc/external_errors.hh>
#include <cassert>
#include <iostream>
#include <cmath>

uint64_t compute_minimum_size_for(std::string& literal) {
  int64_t value = std::stol(literal);
  if (value < 0) {
    value = -value;
  }
  uint64_t bitlength = 1;
  
  // assuming signed numbers;
  while(value > std::pow(2, bitlength - 1)) {
    bitlength *= 2;
  }

  return bitlength;
}

bool check_types(FileDB& file_db, SymbolCache& symbol_cache, TypeCache& type_cache, Declaration* context, Expression* expr) {
  bool type_check_ok = true;
  
  switch (expr->kind) {
    case expression_t::SYMBOL_EXPR:
      {
        std::pair<std::string, Type*>* query_param = symbol_cache.get_parameter(expr);
        if (query_param != nullptr) {
          Type* type = Type::Clone(query_param->second);
          type_cache.expression_types[expr] = type;
        } else {
          Statement* query_stmt = symbol_cache.get_statement(expr);
          if (query_stmt != nullptr) {
            Type* type = Type::Clone(query_stmt->type);
            type_cache.expression_types[expr] = type;
          } else {
            Declaration* query_decl = symbol_cache.get_declaration(context, expr->symbol);
            if (query_decl != nullptr) {
              if (query_decl->kind == declaration_t::TYPE_DECL) {
                Type* type = Type::Clone(query_decl->type);
                type_cache.expression_types[expr] = type;
              } else if (query_decl->kind == declaration_t::FUNCTION_DECL) {
                Type* type = Type::New(type_t::FUNCTION_TYPE);
                type->subtype = Type::Clone(query_decl->type);
                for (auto param : query_decl->parameters) {
                  type->parameters.push_back({param.first, Type::Clone(param.second)});
                }
                type_cache.expression_types[expr] = type;
              } else {// = declaration_t::MODULE_DECL
                throw_module_has_no_type_error(file_db.expression_points[expr], context, expr->symbol);
                type_check_ok = false;
                // for debug
                Type* type = Type::New(type_t::VOID_TYPE);
                type_cache.expression_types[expr] = type;
              }
            } else {
              throw_symbol_should_be_resolved(context, expr->symbol);
            }
          }
        }
      }
      break;
    case expression_t::INTEGER_EXPR:
      {
        Type* type = Type::New(type_t::INTEGER_TYPE);
        type->size = compute_minimum_size_for(expr->literal);
        type->is_signed = true;
        type_cache.expression_types[expr] = type;
      }
      break;
    case expression_t::DOUBLE_EXPR:
      {
        Type* type = Type::New(type_t::DOUBLE_TYPE);
        type->size = 64;
        type_cache.expression_types[expr] = type;
      }
      break;
    case expression_t::BOOLEAN_EXPR:
      {
        Type* type = Type::New(type_t::BOOLEAN_TYPE);
        type_cache.expression_types[expr] = type;
      }
      break;
    case expression_t::NULLPTR_EXPR:
      {
        Type* type = Type::New(type_t::POINTER_TYPE);
        type->subtype = Type::New(type_t::VOID_TYPE);
        type_cache.expression_types[expr] = type;
      }
      break;
    case expression_t::CHARACTER_EXPR:
      {
        Type* type = Type::New(type_t::INTEGER_TYPE);
        type->size = 8;
        type->is_signed = false;
        type_cache.expression_types[expr] = type;
      }
      break;
    case expression_t::STRING_EXPR:
      {
        Type* type = Type::New(type_t::POINTER_TYPE);
        type->subtype = Type::New(type_t::INTEGER_TYPE);
        type->subtype->size = 8;
        type->subtype->is_signed = false;
        type_cache.expression_types[expr] = type;
      }
      break;
    case expression_t::CALL_EXPR:
      {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->callable);
        Type* callable_type = type_cache.expression_types[expr->callable];
        if (callable_type->kind == type_t::FUNCTION_TYPE) {
          if (callable_type->parameters.size() == expr->arguments.size()) {
            for (uint64_t argument_index = 0; argument_index < callable_type->parameters.size(); ++argument_index) {
              Expression* argument = expr->arguments.at(argument_index);
              type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, argument);
              Type* argument_type = type_cache.expression_types[argument];
              Type* parameter_type = callable_type->parameters.at(argument_index).second;
              if (!type_can_be_implicitly_casted_to(symbol_cache, context, argument_type, parameter_type)) {
                throw_type_is_not_implicitly_castable_to(file_db.expression_points[argument], context, argument_type, parameter_type);
                type_check_ok = false;
              }
            }
          } else {
            throw_wrong_parameter_number_error(file_db.expression_points[expr], context, callable_type);
            type_check_ok = false;
          }
          type_cache.expression_types[expr] = Type::Clone(callable_type->subtype);
        } else {
          // for debug purposes
          Type* type = Type::New(type_t::VOID_TYPE);
          type_cache.expression_types[expr] = type;
          throw_type_is_not_callable_error(file_db.expression_points[expr], context, callable_type);
          type_check_ok = false;
        }
      }
      break;
    case expression_t::BINARY_EXPR:
      {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->left);
        Type* left_type = type_cache.expression_types[expr->left];
        Type* original_left_type = left_type;

        if (expr->operator_ == ARR_OP) {
          if (expr->right->kind != expression_t::SYMBOL_EXPR) {
            throw_right_operand_of_arrow_operator_should_be_a_symbol(file_db.expression_points[expr], context);
            type_check_ok = false;
            Type* type = Type::New(type_t::VOID_TYPE);
            type_cache.expression_types[expr] = type;
          } else {
            if (left_type->kind == type_t::SYMBOL_TYPE)
              left_type = resolve_symbol_type(symbol_cache, context, left_type).first;
            if (left_type->kind != type_t::POINTER_TYPE) {
              Type::Print(std::clog << "DEBUG: ", original_left_type) << std::endl;
              throw_left_operand_of_arrow_operator_should_be_a_pointer(file_db.expression_points[expr], context, left_type);
              type_check_ok = false;
              Type* type = Type::New(type_t::VOID_TYPE);
              type_cache.expression_types[expr] = type;
            } else {
              left_type = left_type->subtype;
              if (left_type->kind == type_t::SYMBOL_TYPE)
                left_type = resolve_symbol_type(symbol_cache, context, left_type).first;
              if (left_type->kind != type_t::STRUCT_TYPE) {
                throw_pointed_left_operand_of_arrow_operator_should_be_a_struct(file_db.expression_points[expr], context, left_type);
                type_check_ok = false;
                Type* type = Type::New(type_t::VOID_TYPE);
                type_cache.expression_types[expr] = type;
              } else {
                Type* field_type = Type::ExtractField(left_type, expr->right->symbol);
                if (field_type == nullptr) {
                  throw_struct_has_not_named_field(file_db.expression_points[expr], context, left_type, expr->right->symbol);
                  type_check_ok = false;
                  Type* type = Type::New(type_t::VOID_TYPE);
                  type_cache.expression_types[expr] = type;
                } else {
                  Type* type = Type::Clone(field_type);
                  type_cache.expression_types[expr] = type;
                }
              }
            }
          }
        } else if (expr->operator_ == DOT_OP) {
          if (expr->right->kind != expression_t::SYMBOL_EXPR) {
            throw_right_operand_of_dot_operator_should_be_a_symbol(file_db.expression_points[expr], context);
            type_check_ok = false;
            Type* type = Type::New(type_t::VOID_TYPE);
            type_cache.expression_types[expr] = type;
          } else {
            if (left_type->kind == type_t::SYMBOL_TYPE)
              left_type = resolve_symbol_type(symbol_cache, context, left_type).first;
            if (left_type->kind != type_t::STRUCT_TYPE) {
              throw_left_operand_of_dot_operator_should_be_a_struct(file_db.expression_points[expr], context, left_type);
              type_check_ok = false;
              Type* type = Type::New(type_t::VOID_TYPE);
              type_cache.expression_types[expr] = type;
            } else {
              Type* field_type = Type::ExtractField(left_type, expr->right->symbol);
              if (field_type == nullptr) {
                throw_struct_has_not_named_field(file_db.expression_points[expr], context, left_type, expr->right->symbol);
                type_check_ok = false;
                Type* type = Type::New(type_t::VOID_TYPE);
                type_cache.expression_types[expr] = type;
              } else {
                Type* type = Type::Clone(field_type);
                type_cache.expression_types[expr] = type;
              }
            }
          }
        } else if (expr->operator_ == ASS_OP) {
          type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->right);
          Type* right_type = type_cache.expression_types[expr->right];

          if (!type_can_be_implicitly_casted_to(symbol_cache, context, right_type, left_type)) {
            throw_type_is_not_implicitly_castable_to(file_db.expression_points[expr], context, right_type, left_type);
            type_check_ok = false;
          }
          Type* type = Type::Clone(left_type);
          type_cache.expression_types[expr] = type;
        } else if (is_algebraic_operator(expr->operator_)) {
          type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->right);
          Type* right_type = type_cache.expression_types[expr->right];

          if (types_are_algebraically_manipulable(symbol_cache, context, left_type, right_type)) {
            Type* type = Type::Clone(left_type);
            type_cache.expression_types[expr] = type;
          } else {
            throw_types_cannot_be_algebraically_manipulated_error(file_db.expression_points[expr], context, left_type, right_type);
            type_check_ok = false;
            Type* type = Type::New(type_t::VOID_TYPE);
            type_cache.expression_types[expr] = type;
          }
        } else if (is_logical_operator(expr->operator_)) {
          type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->right);
          Type* right_type = type_cache.expression_types[expr->right];

          if (types_are_logically_manipulable(symbol_cache, context, left_type, right_type)) {
            Type* type = Type::Clone(left_type);
            type_cache.expression_types[expr] = type;
          } else {
            throw_types_cannot_be_logically_manipulated_error(file_db.expression_points[expr], context, left_type, right_type);
            type_check_ok = false;
            Type* type = Type::New(type_t::VOID_TYPE);
            type_cache.expression_types[expr] = type;
          }
        } else {
          assert(false);
        }
      }
      break;
    case expression_t::MONARY_EXPR:
      {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->value);
        Type* value_type = type_cache.expression_types[expr->value];

        switch (expr->operator_) {
          case operator_t::MUL_OP: // *
            {
              if (value_type->kind == type_t::POINTER_TYPE) {
                Type* type = Type::Clone(value_type->subtype);
                type_cache.expression_types[expr] = type;
              } else {
                throw_type_is_not_dereferenceable_error(file_db.expression_points[expr], context, value_type);
                type_check_ok = false;
                Type* type = Type::New(type_t::VOID_TYPE);
                type_cache.expression_types[expr] = type;
              }
            }
            break;
          case operator_t::AND_OP: // &
            {
              Type* type = Type::New(type_t::POINTER_TYPE);
              type->subtype = Type::Clone(value_type);
              type_cache.expression_types[expr] = type;
            }
            break;
          default: // other ops: -, +
            {
              if (is_algebraic_operator(expr->operator_)) {
                if (type_is_algebraically_manipulable(symbol_cache, context, value_type)) {
                  Type* type = Type::Clone(value_type);
                  type_cache.expression_types[expr] = type;
                } else {
                  throw_type_cannot_be_algebraically_manipulated_error(file_db.expression_points[expr], context, value_type);
                  type_check_ok = false;
                  Type* type = Type::New(type_t::VOID_TYPE);
                  type_cache.expression_types[expr] = type;
                }
              } else if (is_logical_operator(expr->operator_)) {
                if (type_is_logically_manipulable(symbol_cache, context, value_type)) {
                  Type* type = Type::Clone(value_type);
                  type_cache.expression_types[expr] = type;
                } else {
                  throw_type_cannot_be_logically_manipulated_error(file_db.expression_points[expr], context, value_type);
                  type_check_ok = false;
                  Type* type = Type::New(type_t::VOID_TYPE);
                  type_cache.expression_types[expr] = type;
                }
              } else {
                assert(false);
              }
            }
            break;
        }
      }
      break;
    case expression_t::SIZEOF_EXPR:
      {
        Type* type = Type::New(type_t::INTEGER_TYPE);
        type->size = 64;
        type->is_signed = false;
        type_cache.expression_types[expr] = type;
      }
      break;
    case expression_t::CAST_EXPR:
      {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->value);
        // Type* value_type = type_cache.expression_types[expr->value];
        // Type* casted_type = expr->type;
        // TODO: check if value_type can be explicitly casted to casted_type
        type_cache.expression_types[expr] = Type::Clone(expr->type);
      }
      break;
    case expression_t::BITCAST_EXPR:
      {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->value);
        // Type* value_type = type_cache.expression_types[expr->value];
        // Type* casted_type = expr->type;
        // by definition, every type can always be explicitly bit-casted to another type
        type_cache.expression_types[expr] = Type::Clone(expr->type);
      }
      break;
  }

  return type_check_ok;
}

bool check_types(FileDB& file_db, SymbolCache& symbol_cache, TypeCache& type_cache, Declaration* context, Statement* stmt) {
  bool type_check_ok = true;

  switch (stmt->kind) {
    case statement_t::LET_STMT:
      if (stmt->expr != nullptr) {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->expr);
      }
      break;
    case statement_t::RETURN_STMT:
      if (stmt->expr != nullptr) {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->expr);
      }
      break;
    case statement_t::BREAK_STMT:
      break;
    case statement_t::CONTINUE_STMT:
      break;
    case statement_t::IF_ELSE_STMT:
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->condition);
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->then);
      if (stmt->else_ != nullptr) {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->else_);
      }
      break;
    case statement_t::WHILE_STMT:
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->condition);
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->body);
      break;
    case statement_t::FOR_STMT:
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->init);
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->condition);
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->step);
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->body);
      break;
    case statement_t::BLOCK_STMT:
      for (Statement* child : stmt->children) {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, child);
      }
      break;
    case statement_t::EXPRESSION_STMT:
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->expr);
      break;
  }

  return type_check_ok;
}

bool check_types(FileDB& file_db, SymbolCache& symbol_cache, TypeCache& type_cache, Declaration* decl) {
  bool type_check_ok = true;

  switch (decl->kind) {
    case declaration_t::FUNCTION_DECL:
      if (decl->body != nullptr) {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, decl, decl->body);
      }
      break;
    case declaration_t::MODULE_DECL:
      for (Declaration* child : decl->children) {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, child);
      }
      break;
    case declaration_t::TYPE_DECL:
      break;
  }

  return type_check_ok;
}
