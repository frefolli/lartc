#include <lartc/resolve/symbol_cache.hh>

std::ostream& SymbolCache::Print(std::ostream& out, SymbolCache& symbol_cache) {
  for (auto decl : symbol_cache.cache) {
    out << "# " << Declaration::QualifiedName(decl.first) << std::endl << std::endl;
    for (auto solved : decl.second) {
      Symbol::Print(out << " - ", solved.first) << " -> ";
      Declaration::PrintShort(out, solved.second) << std::endl;
    }
    out << std::endl << std::endl;
  }
  return out;
}

Declaration* SymbolCache::find_by_going_up(Declaration* context, Symbol& symbol, uint64_t progress) {
  Declaration* query = find_by_going_down(context, symbol);
  while (query == nullptr && context->parent != nullptr) {
    if (context->parent->name == symbol.identifiers.at(symbol.identifiers.size() - 1 - progress)) {
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
  if (context->name == symbol.identifiers.at(symbol.identifiers.size() - 1 - progress)) {
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

Declaration* SymbolCache::find(Declaration* context, Symbol symbol) {
  Declaration* cached = cache[context][symbol];
  if (cached == nullptr) {
    cached = find_by_going_up(context, symbol);
    cache[context][symbol] = cached;
  }
  return cached;
}
