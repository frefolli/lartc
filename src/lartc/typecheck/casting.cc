#include <lartc/typecheck/casting.hh>
#include <lartc/internal_errors.hh>
#include <lartc/external_errors.hh>

std::pair<Type*, Declaration*> resolve_symbol_type(SymbolCache& symbol_cache, Declaration* context, Type* symbol_type) {
  Declaration* decl = symbol_cache.get_or_find_declaration(context, symbol_type->symbol);
  if (decl == nullptr) {
    throw_symbol_should_be_resolved(context, symbol_type->symbol);
  }
  if (decl->kind != declaration_t::TYPE_DECL) {
    throw_attempt_to_use_other_declaration_as_typedef(context, decl);
  }
  if (decl->type->kind == type_t::SYMBOL_TYPE) {
    return resolve_symbol_type(symbol_cache, decl, decl->type);
  }
  return {decl->type, decl};
}

bool types_are_namely_equal(Type* A, Type* B) {
  if (A->kind != B->kind)
    return false;
  bool equals = true;
  switch (A->kind) {
    case type_t::POINTER_TYPE:
      equals &= types_are_namely_equal(A->subtype, B->subtype);
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
          equals &= types_are_namely_equal(A->fields.at(i).second, B->fields.at(i).second);
        }
      }
      break;
    case type_t::SYMBOL_TYPE:
      equals &= A->symbol == B->symbol;
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      equals &= types_are_namely_equal(A->subtype, B->subtype);
      equals &= A->parameters.size() == B->parameters.size();
      if (equals) {
        for (uint64_t i = 0; i < A->parameters.size(); ++i) {
          equals &= types_are_namely_equal(A->parameters.at(i).second, B->parameters.at(i).second);
        }
      }
      break;
  }
  return equals;
}

bool types_are_structurally_equal(SymbolCache& symbol_cache, Declaration* contextA, Type* A, Declaration* contextB, Type* B) {
  if (A->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextA, A);
    A = solved.first;
    contextA = solved.second;
    if (A == nullptr)
      return false;
  }
  if (B->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextB, B);
    B = solved.first;
    contextB = solved.second;
    if (B == nullptr)
      return false;
  }

  if (A->kind != B->kind)
    return false;
  bool equals = true;
  switch (A->kind) {
    case type_t::POINTER_TYPE:
      equals &= types_are_structurally_equal(symbol_cache, contextA, A->subtype, contextB, B->subtype);
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
          equals &= types_are_structurally_equal(symbol_cache, contextA, A->fields.at(i).second, contextB, B->fields.at(i).second);
        }
      }
      break;
    case type_t::SYMBOL_TYPE:
      // err
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      equals &= types_are_structurally_equal(symbol_cache, contextA, A->subtype, contextB, B->subtype);
      equals &= A->parameters.size() == B->parameters.size();
      if (equals) {
        for (uint64_t i = 0; i < A->parameters.size(); ++i) {
          equals &= types_are_structurally_equal(symbol_cache, contextA, A->parameters.at(i).second, contextB, B->parameters.at(i).second);
        }
      }
      break;
  }
  return equals;
}

bool type_can_be_implicitly_casted_to(SymbolCache& symbol_cache, Declaration* context, Type* src, Type* dst) {
  bool implicitly_castable = true;
  switch (dst->kind) {
    case type_t::POINTER_TYPE:
      {
        if (src->kind == type_t::POINTER_TYPE) {
          implicitly_castable &= (src->subtype->kind == VOID_TYPE || types_are_namely_equal(src->subtype, dst->subtype));
        } else {
          implicitly_castable = false;
        }
      }
      break;
    case type_t::VOID_TYPE:
      {
        implicitly_castable &= (src->kind == dst->kind);
      }
      break;
    case type_t::DOUBLE_TYPE:
      {
        if (src->kind == type_t::INTEGER_TYPE || src->kind == type_t::DOUBLE_TYPE) {
          implicitly_castable &= src->size <= dst->size;
        } else {
          implicitly_castable &= types_are_structurally_equal(symbol_cache, context, src, context, dst);
        }
      }
      break;
    case type_t::INTEGER_TYPE:
      {
        if (src->kind == type_t::DOUBLE_TYPE) {
          implicitly_castable &= src->size <= dst->size;
        } else if (src->kind == type_t::INTEGER_TYPE) {
          implicitly_castable &= src->is_signed == dst->is_signed;
          implicitly_castable &= src->size <= dst->size;
        } else if (src->kind == type_t::BOOLEAN_TYPE) {
          // yes
        } else {
          implicitly_castable &= types_are_structurally_equal(symbol_cache, context, src, context, dst);
        }
      }
      break;
    case type_t::STRUCT_TYPE:
      implicitly_castable &= types_are_namely_equal(src, dst);
      break;
    case type_t::SYMBOL_TYPE:
      implicitly_castable &= types_are_structurally_equal(symbol_cache, context, src, context, dst);
      break;
    case type_t::BOOLEAN_TYPE:
        if (src->kind == type_t::BOOLEAN_TYPE) {
          // yes
        } else {
          implicitly_castable &= types_are_structurally_equal(symbol_cache, context, src, context, dst);
        }
      break;
    case type_t::FUNCTION_TYPE:
      implicitly_castable &= types_are_structurally_equal(symbol_cache, context, src, context, dst);
      break;
  }
  return implicitly_castable;
}

bool type_is_algebraically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* type) {
  if (type->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, context, type);
    type = solved.first;
    if (type == nullptr)
      return false;
  }
  bool algebraically_manipulable = true;
  switch (type->kind) {
    case type_t::POINTER_TYPE:
      algebraically_manipulable = false;
      break;
    case type_t::VOID_TYPE:
      algebraically_manipulable = false;
      break;
    case type_t::DOUBLE_TYPE:
      break;
    case type_t::INTEGER_TYPE:
      break;
    case type_t::STRUCT_TYPE:
      algebraically_manipulable = false;
      break;
    case type_t::SYMBOL_TYPE:
      // err
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      algebraically_manipulable = false;
      break;
  }
  return algebraically_manipulable;
}

bool types_are_algebraically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* A, Type* B) {
  Declaration* contextA = context;
  Declaration* contextB = context;

  if (A->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextA, A);
    A = solved.first;
    contextA = solved.second;
    if (A == nullptr)
      return false;
  }

  if (B->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextB, B);
    B = solved.first;
    contextB = solved.second;
    if (B == nullptr)
      return false;
  }

  if (A->kind == type_t::POINTER_TYPE) {
    if (B->kind == type_t::POINTER_TYPE) {
      // cannot add two pointers regaredless of types
      return false;
    } else {
      return type_is_algebraically_manipulable(symbol_cache, contextB, B);
    }
  } else {
    if (B->kind == type_t::POINTER_TYPE) {
      return type_is_algebraically_manipulable(symbol_cache, contextA, A);
    } else {
      return (type_is_algebraically_manipulable(symbol_cache, contextA, A)
              && type_is_algebraically_manipulable(symbol_cache, contextB, B));
    }
  }
}

bool type_is_logically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* type) {
  if (type->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, context, type);
    type = solved.first;
    if (type == nullptr)
      return false;
  }
  bool logically_manipulable = true;
  switch (type->kind) {
    case type_t::POINTER_TYPE:
      break;
    case type_t::VOID_TYPE:
      logically_manipulable = false;
      break;
    case type_t::DOUBLE_TYPE:
      break;
    case type_t::INTEGER_TYPE:
      break;
    case type_t::STRUCT_TYPE:
      logically_manipulable = false;
      break;
    case type_t::SYMBOL_TYPE:
      // err
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      break;
  }
  return logically_manipulable;
}

bool types_are_logically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* A, Type* B) {
  Declaration* contextA = context;
  Declaration* contextB = context;

  if (A->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextA, A);
    A = solved.first;
    contextA = solved.second;
    if (A == nullptr)
      return false;
  }

  if (B->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextB, B);
    B = solved.first;
    contextB = solved.second;
    if (B == nullptr)
      return false;
  }

  return (type_is_logically_manipulable(symbol_cache, contextA, A)
          && type_is_logically_manipulable(symbol_cache, contextB, B));
}
