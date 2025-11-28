#include <cassert>
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
    .parameters = {},
    .is_variadic = false
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
    type->is_variadic = false;

    delete type;
    type = nullptr;
  }
}

Type* Type::Clone(const Type* other) {
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
  type->is_variadic = true;
  return type;
}

std::ostream& Type::Print(std::ostream& out, const Type* type, std::uintmax_t tabulation) {
  assert(type != nullptr);
  tabulate(out, tabulation);
  bool first;
  switch (type->kind) {
    case type_t::INTEGER_TYPE:
      return out << "integer<" << type->size << "," << std::boolalpha << type->is_signed << std::noboolalpha << ">";
    case type_t::DOUBLE_TYPE:
      return out << "double<" << type->size << ">";
    case type_t::BOOLEAN_TYPE:
      return out << "bool";
    case type_t::POINTER_TYPE:
      return Type::Print(out << "&", type->subtype);
    case type_t::ARRAY_TYPE:
      return Type::Print(out << "[", type->subtype) << ", " << type->size << "]";
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
      if (type->is_variadic) {
        if (type->parameters.size() > 0) {
          out << ", ";
        }
        out << "...";
      }
      return Type::Print(out << ") -> ", type->subtype, 0);
  }
  return out;
}

Type* Type::ExtractField(const Type* struct_type, const Symbol& name) {
  std::intmax_t index = ExtractFieldIndex(struct_type, name);
  if (index != -1)
    return struct_type->fields[index].second;
  return nullptr;
}

std::intmax_t Type::ExtractFieldIndex(const Type* struct_type, const Symbol& name) {
  for (std::uintmax_t index = 0; index < struct_type->fields.size(); ++index) {
    if (struct_type->fields[index].first == name.identifiers.front()) {
      return index;
    }
  }
  return -1;
}
