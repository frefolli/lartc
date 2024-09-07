#include <lartc/typecheck/size_cache.hh>

void SizeCache::add_declaration(SymbolCache& symbol_cache, Declaration* decl) {
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      for (auto child : decl->children) {
        add_declaration(symbol_cache, child);
      }
      break;
    case declaration_t::TYPE_DECL:
      if (!sizes.contains(decl)) {
        staging[decl] = true;
        sizes[decl] = compute_size(symbol_cache, decl, decl->type);
        staging[decl] = false;
      }
      break;
    case declaration_t::FUNCTION_DECL:
      break;
  }
}

uint64_t SizeCache::compute_size(SymbolCache& symbol_cache, Declaration* context, Type* type) {
  uint64_t size = 0;
  switch (type->kind) {
    case type_t::POINTER_TYPE:
      size = 64;
      break;
    case type_t::VOID_TYPE:
      break;
    case type_t::DOUBLE_TYPE:
      size = type->size;
      break;
    case type_t::INTEGER_TYPE:
      size = type->size;
      break;
    case type_t::STRUCT_TYPE:
      for (auto item : type->fields) {
        size += compute_size(symbol_cache, context, item.second);
      }
      break;
    case type_t::SYMBOL_TYPE:
      {
        Declaration* decl = symbol_cache.get_declaration(context, type->symbol);
        if (staging[decl]) {
          // throw type system error
        } else {
          if (!sizes.contains(decl)) {
            add_declaration(symbol_cache, decl);
          }
          size += sizes[decl];
        }
      }
      break;
    case type_t::BOOLEAN_TYPE:
      size = 1;
      break;
    case type_t::FUNCTION_TYPE:
      size = 64;
      break;
  }
  return 0;
}
