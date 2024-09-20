#include <lartc/typecheck/size_cache.hh>
  
std::ostream& SizeCache::Print(std::ostream& out, SizeCache& size_cache) {
  out << "# Size Cache" << std::endl << std::endl;
  for (auto item : size_cache.sizes) {
    Declaration::PrintShort(out <<  " - ", item.first) << ": " << item.second << std::endl;
  }
  return out;
}

uint64_t SizeCache::compute_size_of(SymbolCache& symbol_cache, Declaration* scope, Type* type) {
  uint64_t size = 0;
  switch (type->kind) {
    case INTEGER_TYPE:
      {
        size += type->size;
        break;
      }
    case DOUBLE_TYPE:
      {
        size += type->size;
        break;
      }
    case BOOLEAN_TYPE:
      {
        size += 1;
        break;
      }
    case POINTER_TYPE:
      {
        size += 64;
        break;
      }
    case SYMBOL_TYPE:
      {
        Declaration* decl = symbol_cache.get_declaration(scope, type->symbol);
        size += sizes[decl];
        break;
      }
    case VOID_TYPE:
      {
        break;
      }
    case STRUCT_TYPE:
      {
        for (auto item : type->fields) {
          size += compute_size_of(symbol_cache, scope, item.second);
        }
        break;
      }
    case FUNCTION_TYPE:
      {
        size += 64;
        break;
      }
  }
  return size;
}
