#include <lartc/ast/type.hh>
#include <lartc/internal_errors.hh>
#include <ios>

Type* Type::New(type_t kind) {
  return new Type {kind, 0, false, nullptr, ""};
}

void Type::Delete(Type*& type) {
  if (type != nullptr) {
    Type::Delete(type->subtype);
    type->size = 0;
    type->is_signed = false;
    type->identifier = "";

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

std::ostream& Type::Print(std::ostream& out, Type* type, uint64_t tabulation) {
  tabulate(out, tabulation);
  bool first;
  switch (type->kind) {
    case type_t::INTEGER_TYPE:
      return out << "integer_type<" << type->size << "," << std::boolalpha << type->is_signed << std::noboolalpha << ">";
    case type_t::DOUBLE_TYPE:
      return out << "double_type<" << type->size << ">";
    case type_t::BOOLEAN_TYPE:
      return out << "boolean_type";
    case type_t::POINTER_TYPE:
      return Type::Print(out << "pointer_type<", type->subtype) << ">";
    case type_t::IDENTIFIER_TYPE:
      return out << type->identifier;
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
