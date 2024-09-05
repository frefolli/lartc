#include <ios>
#include <lartc/ast/type.hh>
#include <lartc/internal_errors.hh>

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
  if (type->subtype != nullptr) {
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
