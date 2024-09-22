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
  if (decl->type == nullptr) {
    std::exit(1);
  }
  if (decl->type->kind == type_t::SYMBOL_TYPE) {
    return resolve_symbol_type(symbol_cache, decl, decl->type);
  }
  return {decl->type, decl};
}

bool types_are_namely_equal(SymbolCache& symbol_cache, Declaration* contextA, Type* A, Declaration* contextB, Type* B) {
  if (A->kind != B->kind)
    return false;
  bool equals = true;
  switch (A->kind) {
    case type_t::POINTER_TYPE:
      equals &= types_are_namely_equal(symbol_cache, contextA, A->subtype, contextB, B->subtype);
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
          equals &= types_are_namely_equal(symbol_cache, contextA, A->fields.at(i).second, contextB, B->fields.at(i).second);
        }
      }
      break;
    case type_t::SYMBOL_TYPE:
      if (A->symbol != B->symbol) {
        auto solvedA = resolve_symbol_type(symbol_cache, contextA, A);
        auto solvedB = resolve_symbol_type(symbol_cache, contextB, B);
        equals &= (solvedA.second == solvedB.second);
      }
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      equals &= types_are_namely_equal(symbol_cache, contextA, A->subtype, contextB, B->subtype);
      equals &= A->parameters.size() == B->parameters.size();
      if (equals) {
        for (uint64_t i = 0; i < A->parameters.size(); ++i) {
          equals &= types_are_namely_equal(symbol_cache, contextA, A->parameters.at(i).second, contextB, B->parameters.at(i).second);
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
  }
  if (B->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextB, B);
    B = solved.first;
    contextB = solved.second;
  }

  if (A == B) {
    // resolved to the same definition;
    // are equal by definition
    return true;
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

bool types_are_structurally_compatible(SymbolCache& symbol_cache, Declaration* contextSrc, Type* Src, Declaration* contextDst, Type* Dst) {
  if (Src->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextSrc, Src);
    Src = solved.first;
    contextSrc = solved.second;
  }
  if (Dst->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextDst, Dst);
    Dst = solved.first;
    contextDst = solved.second;
  }

  if (Src == Dst) {
    // resolved to the same definition;
    // are equal by definition
    return true;
  }

  if (Src->kind != Dst->kind)
    return false;
  bool compatibles = true;
  switch (Src->kind) {
    case type_t::POINTER_TYPE:
      compatibles &= types_are_structurally_compatible(symbol_cache, contextSrc, Src->subtype, contextDst, Dst->subtype);
      break;
    case type_t::VOID_TYPE:
      break;
    case type_t::DOUBLE_TYPE:
      compatibles &= Src->size <= Dst->size;
      break;
    case type_t::INTEGER_TYPE:
      compatibles &= Src->size <= Dst->size;
      // compatibles &= A->is_signed == Dst->is_signed;
      break;
    case type_t::STRUCT_TYPE:
      compatibles &= Src->fields.size() == Dst->fields.size();
      if (compatibles) {
        for (uint64_t i = 0; i < Src->fields.size(); ++i) {
          compatibles &= types_are_structurally_compatible(symbol_cache, contextSrc, Src->fields.at(i).second, contextDst, Dst->fields.at(i).second);
        }
      }
      break;
    case type_t::SYMBOL_TYPE:
      // err
      break;
    case type_t::BOOLEAN_TYPE:
      break;
    case type_t::FUNCTION_TYPE:
      compatibles &= types_are_structurally_compatible(symbol_cache, contextSrc, Src->subtype, contextDst, Dst->subtype);
      compatibles &= Src->parameters.size() == Dst->parameters.size();
      if (compatibles) {
        for (uint64_t i = 0; i < Src->parameters.size(); ++i) {
          compatibles &= types_are_structurally_compatible(symbol_cache, contextSrc, Src->parameters.at(i).second, contextDst, Dst->parameters.at(i).second);
        }
      }
      break;
  }
  return compatibles;
}

bool type_can_be_implicitly_casted_to(SymbolCache& symbol_cache, Declaration* context, Type* src, Type* dst) {
  Declaration* context_src = context;
  if (src->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, context_src, src);
    src = solved.first;
    context_src = solved.second;
  }
  Declaration* context_dst = context;
  if (dst->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, context_dst, dst);
    dst = solved.first;
    context_dst = solved.second;
  }

  if (src == dst) {
    // resolved to the same definition;
    // are equal by definition
    return true;
  }

  bool implicitly_castable = true;
  switch (dst->kind) {
    case type_t::POINTER_TYPE:
      {
        if (src->kind == type_t::POINTER_TYPE) {
          implicitly_castable &= (src->subtype->kind == VOID_TYPE || dst->subtype->kind == VOID_TYPE || types_are_structurally_compatible(symbol_cache, context, src->subtype, context, dst->subtype));
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
          implicitly_castable &= types_are_structurally_compatible(symbol_cache, context, src, context, dst);
        }
      }
      break;
    case type_t::INTEGER_TYPE:
      {
        if (src->kind == type_t::DOUBLE_TYPE) {
          implicitly_castable &= src->size <= dst->size;
        } else if (src->kind == type_t::INTEGER_TYPE) {
          // implicitly_castable &= src->is_signed == dst->is_signed;
          implicitly_castable &= src->size <= dst->size;
        } else if (src->kind == type_t::BOOLEAN_TYPE) {
          // yes
        } else {
          implicitly_castable &= types_are_structurally_compatible(symbol_cache, context, src, context, dst);
        }
      }
      break;
    case type_t::STRUCT_TYPE:
      implicitly_castable &= types_are_namely_equal(symbol_cache, context, src, context, dst);
      break;
    case type_t::SYMBOL_TYPE:
      implicitly_castable &= types_are_structurally_compatible(symbol_cache, context, src, context, dst);
      break;
    case type_t::BOOLEAN_TYPE:
        if (src->kind == type_t::BOOLEAN_TYPE) {
          // yes
        } else {
          implicitly_castable &= types_are_structurally_compatible(symbol_cache, context, src, context, dst);
        }
      break;
    case type_t::FUNCTION_TYPE:
      implicitly_castable &= types_are_structurally_compatible(symbol_cache, context, src, context, dst);
      break;
  }
  return implicitly_castable;
}

bool type_is_algebraically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* type) {
  if (type->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, context, type);
    type = solved.first;
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
  }

  if (B->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextB, B);
    B = solved.first;
    contextB = solved.second;
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
  }

  if (B->kind == type_t::SYMBOL_TYPE) {
    auto solved = resolve_symbol_type(symbol_cache, contextB, B);
    B = solved.first;
    contextB = solved.second;
  }

  return (type_is_logically_manipulable(symbol_cache, contextA, A)
          && type_is_logically_manipulable(symbol_cache, contextB, B));
}
