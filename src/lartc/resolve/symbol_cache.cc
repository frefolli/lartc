#include <lartc/resolve/symbol_cache.hh>

std::ostream& SymbolCache::Print(std::ostream& out, SymbolCache& symbol_cache) {
  out << "# Symbol Cache" << std::endl << std::endl;
  out << "## Globals" << std::endl << std::endl;
  for (auto decl : symbol_cache.globals) {
    out << "### " << Declaration::QualifiedName(decl.first) << std::endl << std::endl;
    for (auto solved : decl.second) {
      Symbol::Print(out << " - ", solved.first) << " -> ";
      Declaration::PrintShort(out, solved.second) << std::endl;
    }
    out << std::endl;
  }

  out << "## Locals" << std::endl << std::endl;
  for (auto solved : symbol_cache.locals) {
    Expression::Print(out << " - ", solved.first) << " -> ";
    Statement::PrintShort(out, solved.second) << std::endl;
  }
  out << std::endl;

  out << "## Parameters" << std::endl << std::endl;
  for (auto solved : symbol_cache.parameters) {
    Expression::Print(out << " - ", solved.first) << " -> ";
    out << solved.second->first << ": ";
    Type::Print(out, solved.second->second) << std::endl;
  }
  return out;
}

Declaration* SymbolCache::find_by_going_up(Declaration* context, Symbol& symbol, uint64_t progress) {
  Declaration* query = find_by_going_down(context, symbol);
  while (query == nullptr && context->parent != nullptr) {
    if (context->parent->name == symbol.identifiers.at(progress)) {
      progress += 1;
      if (symbol.identifiers.size() == progress) {
        return context->parent;
      } else {
        return find_by_going_down(context->parent, symbol, progress);
      }
    } else {
      if (context->parent->kind == declaration_t::MODULE_DECL) {
        for (Declaration* child : context->parent->children) {
          if (child != context) {
            query = find_by_going_down(child, symbol, progress);
            if (query != nullptr) {
              return query;
            }
          }
        }
      }
      context = context->parent;
    }
  }
  return query;
}

Declaration* SymbolCache::find_by_going_down(Declaration* context, Symbol& symbol, uint64_t progress) {
  if (context->name == symbol.identifiers.at(progress)) {
    progress += 1;
    if (symbol.identifiers.size() == progress) {
      return context;
    }
  }
  if (context->kind == declaration_t::MODULE_DECL) {
    for (Declaration* child : context->children) {
      Declaration* query = find_by_going_down(child, symbol, progress);
      if (query != nullptr) {
        return query;
      }
    }
  }
  return nullptr;
}

Declaration* SymbolCache::get_or_find_declaration(Declaration* context, Symbol& symbol) {
  Declaration* query = get_declaration(context, symbol);
  if (query != nullptr)
    return query;

  query = find_by_going_up(context, symbol);
  // i want to signal that no definition is found from scope of context
  globals[context][symbol] = query;
  return query;
}

Declaration* SymbolCache::get_declaration(Declaration* context, Symbol& symbol) const {
  if (globals.contains(context)) {
    if (globals.at(context).contains(symbol)) {
      return globals.at(context).at(symbol);
    }
  }
  return nullptr;
}
  
Statement* SymbolCache::get_or_find_statement(SymbolStack& symbol_stack, Expression* expression, Symbol& symbol) {
  Statement* query = get_statement(expression);
  if (query != nullptr)
    return query;

  query = symbol_stack.get(symbol);
  if (query != nullptr) {
    locals[expression] = query;
  }
  return query;
}

Statement* SymbolCache::get_statement(Expression* context) const {
  if (locals.contains(context)) {
    return locals.at(context);
  }
  return nullptr;
}

inline std::pair<std::string, Type*>* find_parameter(Declaration* context, Symbol& symbol) {
  for (auto it = context->parameters.begin(); it != context->parameters.end(); ++it) {
    if (it->first == symbol.identifiers.front()) {
      return &(*it);
    }
  }
  return nullptr;
}
  
std::pair<std::string, Type*>* SymbolCache::get_or_find_parameter(Declaration* context, Expression* expression, Symbol& symbol) {
  std::pair<std::string, Type*>* query = get_parameter(expression);
  if (query != nullptr)
    return query;

  query = find_parameter(context, symbol);
  if (query != nullptr) {
    parameters[expression] = query;
  }
  return query;
}

std::pair<std::string, Type*>* SymbolCache::get_parameter(Expression* context) const {
  if (parameters.contains(context)) {
    return parameters.at(context);
  }
  return nullptr;
}
