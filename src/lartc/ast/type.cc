#include <lartc/ast/type.hh>
#include <lartc/internal_errors.hh>
#include <ios>
#include <iostream>

Type* Type::New(type_t kind) {
  return new Type {
    .kind = kind,
    .size = 0,
    .is_signed = false,
    .subtype = nullptr,
    .symbol = {},
    .fields = {},
    .parameters = {}
  };
}

void Type::Delete(Type*& type) {
  if (type != nullptr) {
    Type::Delete(type->subtype);
    type->size = 0;
    type->is_signed = false;
    type->symbol = {};

    for (std::pair<std::string, Type*>& item : type->fields) {
      Type::Delete(item.second);
    }
    type->fields = {};

    for (std::pair<std::string, Type*>& item : type->parameters) {
      Type::Delete(item.second);
    }
    type->parameters = {};

    delete type;
    type = nullptr;
  }
}

Type* Type::Clone(Type* other) {
  if (other == nullptr) {
    throw_internal_error(ATTEMPT_TO_CLONE_NULLPTR_AS_TYPE, MSG(""));
  }
  Type* type = Type::New(other->kind);
  type->size = other->size;
  type->is_signed = other->is_signed;
  if (other->subtype != nullptr) {
    type->subtype = Type::Clone(other->subtype);
  }
  type->symbol = other->symbol;
  for (auto item : other->fields) {
    type->fields.push_back({item.first, Type::Clone(item.second)});
  }
  for (auto item : other->parameters) {
    type->parameters.push_back({item.first, Type::Clone(item.second)});
  }
  return type;
}

std::ostream& Type::Print(std::ostream& out, Type* type, uint64_t tabulation) {
  tabulate(out, tabulation);
  bool first;
  switch (type->kind) {
    case type_t::INTEGER_TYPE:
      return out << "int<" << type->size << "," << std::boolalpha << type->is_signed << std::noboolalpha << ">";
    case type_t::DOUBLE_TYPE:
      return out << "double<" << type->size << ">";
    case type_t::BOOLEAN_TYPE:
      return out << "bool";
    case type_t::POINTER_TYPE:
      return Type::Print(out << "&", type->subtype);
    case type_t::SYMBOL_TYPE:
      return Symbol::Print(out, type->symbol);
    case type_t::VOID_TYPE:
      return out << "void";
    case type_t::STRUCT_TYPE:
      out << "struct {";
      first = true;
      for (std::pair<std::string, Type*> item : type->fields) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        Type::Print(out << item.first << ": ", item.second);
      }
      return out << "}";
    case type_t::FUNCTION_TYPE:
      out << "(";
      first = true;
      for (std::pair<std::string, Type*> item : type->parameters) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        Type::Print(out << item.first << ": ", item.second);
      }
      return Type::Print(out << ") -> ", type->subtype, 0);
  }
  return out;
}

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

/*
uint64_t Type::Size(Type* type) {
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
        size += Type::Size(item.second);
      }
      break;
    case type_t::SYMBOL_TYPE:
      // TODO: RESOLVE
      size &= 64;
      break;
    case type_t::BOOLEAN_TYPE:
      size = 1;
      break;
    case type_t::FUNCTION_TYPE:
      size = 64;
      break;
  }
  return size;
}
*/

Type* Type::ExtractField(Type* struct_type, Symbol& name) {
  for (auto item : struct_type->fields) {
    if (item.first == name.identifiers.front()) {
      return item.second;
    }
  }
  return nullptr;
}
