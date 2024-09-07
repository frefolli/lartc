#include <lartc/typecheck/size_cache.hh>

// TODO: remove or refactor
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
            // add_declaration(symbol_cache, decl);
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
