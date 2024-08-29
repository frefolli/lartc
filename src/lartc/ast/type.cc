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
  tabulate(out, tabulation) << type->kind;
  switch (type->kind) {
    case type_t::INTEGER_TYPE:
      return out << " " << type->size << " " << std::boolalpha << type->is_signed << std::noboolalpha << ";";
    case type_t::DOUBLE_TYPE:
      return out << " " << type->size << " " << ";";
    case type_t::BOOLEAN_TYPE:
      return out << ";";
    case type_t::POINTER_TYPE:
      return Type::Print(out << " of {" << std::endl, type->subtype, tabulation + 1) << std::endl << "}";
    case type_t::IDENTIFIER_TYPE:
      return out << " ref " << type->identifier << ";";
    case type_t::VOID_TYPE:
      return out << ";";
    case type_t::STRUCT_TYPE:
      return out << ";";
    case type_t::FUNCTION_TYPE:
      return out << ";";
  }
  return out;
}
