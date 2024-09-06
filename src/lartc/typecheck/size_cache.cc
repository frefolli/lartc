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

/*
bool Type::Equal(Type* A, Type* B) {
  if (A->kind != B->kind)
    return false;
  bool equals = true;
  switch (A->kind) {
    case type_t::POINTER_TYPE:
      equals &= Type::Equal(A->subtype, B->subtype);
      break;
    case type_t::VOID_TYPE:
      break;
    case type_t::DOUBLE_TYPE:
      equals &= A->size == B->size;
      break;
    case type_t::INTEGER_TYPE:
      equals &= A->size == B->size;
      equals &= A->is_signed == B->is_signed;
      break;
    case type_t::STRUCT_TYPE:
      equals &= A->fields.size() == B->fields.size();
      if (equals) {
        for (uint64_t i = 0; i < A->fields.size(); ++i) {
          equals &= Type::Equal(A->fields.at(i).second, B->fields.at(i).second);
        }
      }
      break;
    case type_t::SYMBOL_TYPE:
      equals &= A->symbol == B->symbol;
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      equals &= Type::Equal(A->subtype, B->subtype);
      equals &= A->parameters.size() == B->parameters.size();
      if (equals) {
        for (uint64_t i = 0; i < A->parameters.size(); ++i) {
          equals &= Type::Equal(A->parameters.at(i).second, B->parameters.at(i).second);
        }
      }
      break;
  }
  return equals;
}
*/

/*
bool Type::CanBeImplicitlyCastedTo(Type* src, Type* dst) {
  bool implicitly_castable = true;
  switch (src->kind) {
    case type_t::POINTER_TYPE:
      implicitly_castable &= (src->subtype->kind == VOID_TYPE || Type::Equal(src->subtype, dst->subtype));
      break;
    case type_t::VOID_TYPE:
      break;
    case type_t::DOUBLE_TYPE:
      implicitly_castable &= src->size <= dst->size;
      break;
    case type_t::INTEGER_TYPE:
      implicitly_castable &= src->size <= dst->size;
      implicitly_castable &= src->is_signed == dst->is_signed;
      break;
    case type_t::STRUCT_TYPE:
      implicitly_castable &= Type::Equal(src, dst);
      break;
    case type_t::SYMBOL_TYPE:
      implicitly_castable &= Type::Equal(src, dst);
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      implicitly_castable &= Type::Equal(src, dst);
      break;
  }
  return implicitly_castable;
}
*/
