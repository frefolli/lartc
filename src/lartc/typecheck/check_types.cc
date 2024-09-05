#include <lartc/typecheck/check_types.hh>
#include <lartc/internal_errors.hh>
#include <lartc/external_errors.hh>

// Internal Errors
void symbol_should_be_resolved() {
  throw_internal_error(NOT_IMPLEMENTED, MSG(__FILE__ << ", " << __LINE__));
}

// External Errors
void type_is_not_dereferenceable() {
  throw_internal_error(NOT_IMPLEMENTED, MSG(__FILE__ << ", " << __LINE__));
}

void module_has_no_type() {
  throw_internal_error(NOT_IMPLEMENTED, MSG(__FILE__ << ", " << __LINE__));
}

void type_cannot_be_algebrically_manipulated() {
  throw_internal_error(NOT_IMPLEMENTED, MSG(__FILE__ << ", " << __LINE__));
}

void type_is_not_callable() {
  throw_internal_error(NOT_IMPLEMENTED, MSG(__FILE__ << ", " << __LINE__));
}

void wrong_parameter_number() {
  throw_internal_error(NOT_IMPLEMENTED, MSG(__FILE__ << ", " << __LINE__));
}

bool check_types(FileDB& file_db, SymbolCache& symbol_cache, TypeCache& type_cache, Declaration* context, Type* type) {
  bool type_check_ok = true;
  /* THIS FUNCTION IS A STUB FOR SOME CHECKS THAT CAN BE DONE ON TYPE CONSISTENCY AND SAFE RECURSION */
  return type_check_ok;
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
                // for debug purposes
                Type* type = Type::New(type_t::VOID_TYPE);
                type_cache.expression_types[expr] = type;
                module_has_no_type();
                type_check_ok = false;
              }
            } else {
              symbol_should_be_resolved();
            }
          }
        }
      }
      break;
    case expression_t::INTEGER_EXPR:
      {
        Type* type = Type::New(type_t::INTEGER_TYPE);
        type->size = 64;
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
              type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->arguments.at(argument_index));
              Type* parameter_type = callable_type->parameters.at(argument_index).second;
              Type* argument_type = type_cache.expression_types[expr->arguments.at(argument_index)];
              // TODO: check if argument_type can be implicitly casted to parameter_type
            }
          } else {
            wrong_parameter_number();
            type_check_ok = false;
          }
          type_cache.expression_types[expr] = Type::Clone(callable_type->subtype);
        } else {
          // for debug purposes
          Type* type = Type::New(type_t::VOID_TYPE);
          type_cache.expression_types[expr] = type;
          type_is_not_callable();
          type_check_ok = false;
        }
      }
      break;
    case expression_t::BINARY_EXPR:
      {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->left);
        Type* left_type = type_cache.expression_types[expr->left];

        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->right);
        Type* right_type = type_cache.expression_types[expr->right];

          // TODO: implement
          Type* type = Type::New(type_t::VOID_TYPE);
          type_cache.expression_types[expr] = type;
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
                type_is_not_dereferenceable();
                type_check_ok = false;
              }
            }
            break;
          case operator_t::AND_OP: // &
            {
              Type* type = Type::New(type_t::VOID_TYPE);
              type->subtype = Type::Clone(value_type);
              type_cache.expression_types[expr] = type;
            }
            break;
          default: // other ops: -, +
            {
              if (value_type->kind == type_t::INTEGER_TYPE &&
                  value_type->kind == type_t::DOUBLE_TYPE &&
                  value_type->kind == type_t::POINTER_TYPE) {
                Type* type = Type::Clone(value_type);
                type_cache.expression_types[expr] = type;
              } else {
                type_cannot_be_algebrically_manipulated();
                type_check_ok = false;
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
        Type* value_type = type_cache.expression_types[expr->value];
        Type* casted_type = expr->type;
        // TODO: check if value_type can be explicitly casted to casted_type
        type_cache.expression_types[expr] = Type::Clone(expr->type);
      }
      break;
    case expression_t::BITCAST_EXPR:
      {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, expr->value);
        Type* value_type = type_cache.expression_types[expr->value];
        Type* casted_type = expr->type;
        // TODO: check if value_type can be explicitly casted to casted_type
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
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, context, stmt->else_);
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
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, decl, decl->type);
      for (auto param : decl->parameters) {
        type_check_ok &= check_types(file_db, symbol_cache, type_cache, decl, param.second);
      }
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
      type_check_ok &= check_types(file_db, symbol_cache, type_cache, decl, decl->type);
      break;
  }

  return type_check_ok;
}
