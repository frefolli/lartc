#include <lartc/resolve/resolve_symbols.hh>
#include <lartc/external_errors.hh>

inline bool resolve_symbol_or_throw_error(FileDB& file_db, SymbolCache &symbol_cache, Declaration* context, Symbol& symbol) {
  Declaration* query = symbol_cache.get_or_find_declaration(context, symbol);
  if (query == nullptr) {
    throw_name_resolution_error(file_db.symbol_points[&symbol], context, symbol);
    return false;
  }
  return true;
}

inline bool resolve_symbol_or_throw_error(FileDB& file_db, SymbolCache &symbol_cache, SymbolStack& symbol_stack, Declaration* context, Expression* expression, Symbol& symbol) {
  if (symbol.identifiers.size() == 1) {
    Statement* query = symbol_cache.get_or_find_statement(symbol_stack, expression, symbol);
    if (query != nullptr) {
      return true;
    } else {
      std::pair<std::string, Type*>* query = symbol_cache.get_or_find_parameter(context, expression, symbol);
      if (query != nullptr)
        return true;
    }
  }
  bool ok = resolve_symbol_or_throw_error(file_db, symbol_cache, context, symbol);
  return ok;
}

bool resolve_symbols(FileDB& file_db, SymbolCache &symbol_cache, Declaration* context, Type* type);
inline bool resolve_symbols(FileDB& file_db, SymbolCache& symbol_cache, Declaration* context, std::vector<std::pair<std::string, Type*>>& parameter_or_field_list) {
  bool resolution_ok = true;
  for (std::pair<std::string, Type*> item : parameter_or_field_list) {
    resolution_ok &= resolve_symbols(file_db, symbol_cache, context, item.second);
  }
  return resolution_ok;
}

bool resolve_symbols(FileDB& file_db, SymbolCache &symbol_cache, Declaration* context, Type* type) {
  bool resolution_ok = true;

  switch (type->kind) {
    case type_t::SYMBOL_TYPE:
      resolution_ok &= resolve_symbol_or_throw_error(file_db, symbol_cache, context, type->symbol);
      break;
    case type_t::POINTER_TYPE:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, context, type->subtype);
      break;
    case type_t::STRUCT_TYPE:
      resolution_ok &= resolve_symbols(file_db, symbol_cache,  context, type->fields);
      break;
    case type_t::FUNCTION_TYPE:
      resolution_ok &= resolve_symbols(file_db, symbol_cache,  context, type->parameters);
      resolution_ok &= resolve_symbols(file_db, symbol_cache, context, type->subtype);
      break;
    case type_t::VOID_TYPE:
      break;
    case type_t::INTEGER_TYPE:
      break;
    case type_t::DOUBLE_TYPE:
      break;
    case type_t::BOOLEAN_TYPE:
      break;
  }

  return resolution_ok;
}

bool resolve_symbols(FileDB& file_db, SymbolCache &symbol_cache, SymbolStack& symbol_stack, Declaration* context, Expression* expr) {
  bool resolution_ok = true;

  switch (expr->kind) {
    case expression_t::SYMBOL_EXPR:
      resolve_symbol_or_throw_error(file_db, symbol_cache, symbol_stack, context, expr, expr->symbol);
      break;
    case expression_t::CALL_EXPR:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, expr->callable);
      for (Expression* arg : expr->arguments) {
        resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, arg);
      }
      break;
    case expression_t::CAST_EXPR:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, context, expr->type);
      break;
    case expression_t::BITCAST_EXPR:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, context, expr->type);
      break;
    case expression_t::SIZEOF_EXPR:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, context, expr->type);
      break;
    case expression_t::BINARY_EXPR:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, expr->left);
      if (expr->operator_ != DOT_OP) {
        resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, expr->right);
      }
      break;
    case expression_t::MONARY_EXPR:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, expr->value);
      break;
    case expression_t::INTEGER_EXPR:
      break;
    case expression_t::DOUBLE_EXPR:
      break;
    case expression_t::NULLPTR_EXPR:
      break;
    case expression_t::BOOLEAN_EXPR:
      break;
    case expression_t::CHARACTER_EXPR:
      break;
    case expression_t::STRING_EXPR:
      break;
  }

  return resolution_ok;
}

bool resolve_symbols(FileDB& file_db, SymbolCache &symbol_cache, SymbolStack& symbol_stack, Declaration* context, Statement* stmt) {
  bool resolution_ok = true;

  switch (stmt->kind) {
    case statement_t::BLOCK_STMT:
      symbol_stack.open_scope();
      for (Statement* child : stmt->children) {
        resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, child);
      }
      symbol_stack.close_scope();
      break;
    case statement_t::FOR_STMT:
      symbol_stack.open_scope();
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->init);
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->condition);
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->step);
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->body);
      symbol_stack.close_scope();
     break;
    case statement_t::LET_STMT:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, context, stmt->type);
      if (stmt->expr != nullptr) {
        resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->expr);
      }
      symbol_stack.set(stmt->name, stmt);
      break;
    case statement_t::RETURN_STMT:
      if (stmt->expr != nullptr) {
        resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->expr);
      }
      break;
    case statement_t::IF_ELSE_STMT:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->condition);
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->then);
      if (stmt->else_ != nullptr) {
        resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->else_);
      }
      break;
    case statement_t::WHILE_STMT:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->condition);
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->body);
      break;
    case statement_t::EXPRESSION_STMT:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, context, stmt->expr);
      break;
    case statement_t::BREAK_STMT:
      break;
    case statement_t::CONTINUE_STMT:
      break;
  }

  return resolution_ok;
}

bool resolve_symbols(FileDB& file_db, SymbolCache& symbol_cache, Declaration* decl) {
  bool resolution_ok = true;

  switch (decl->kind) {
    case declaration_t::TYPE_DECL:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, decl, decl->type);
      break;
    case declaration_t::FUNCTION_DECL:
      resolution_ok &= resolve_symbols(file_db, symbol_cache, decl, decl->type);
      resolution_ok &= resolve_symbols(file_db, symbol_cache, decl, decl->parameters);
      if (decl->body != nullptr) {
        SymbolStack symbol_stack;
        symbol_stack.open_scope();
        resolution_ok &= resolve_symbols(file_db, symbol_cache, symbol_stack, decl, decl->body);
        symbol_stack.close_scope();
      }
      break;
    case declaration_t::MODULE_DECL:
      for (Declaration* child : decl->children) {
        resolution_ok &= resolve_symbols(file_db, symbol_cache, child);
      }
      break;
  }
  return resolution_ok;
}
