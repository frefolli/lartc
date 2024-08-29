#include <lartc/ast/declaration.hh>
#include <lartc/internal_errors.hh>

Declaration* Declaration::New(declaration_t kind) {
  return new Declaration {kind, {}, "", nullptr, nullptr, {}};
}

void Declaration::Delete(Declaration*& decl) {
  if (decl != nullptr) {
    for (Declaration*& child : decl->children) {
      Declaration::Delete(child);
    }
    decl->children.clear();
    decl->name.clear();
    for (std::pair<std::string, Type*>& item : decl->parameters) {
      Type::Delete(item.second);
    }
    decl->parameters.clear();
    delete decl;
    decl = nullptr;
  }
}

std::ostream& Declaration::Print(std::ostream& out, Declaration* decl, uint64_t tabulation) {
  tabulate(out, tabulation) << decl->kind << " " << decl->name;
  bool first;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      out << " {" << std::endl;
      for (Declaration* child : decl->children) {
        Declaration::Print(out, child, tabulation + 1);
        out << std::endl;
      }
      return out << "}";
    case declaration_t::FUNCTION_DECL:
      out << "(";
      first = true;
      for (std::pair<std::string, Type*> item : decl->parameters) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        Type::Print(out << item.first << ": ", item.second);
      }
      return Type::Print(out << ") -> ", decl->type, 0);
    case declaration_t::TYPE_DECL:
      Type::Print(out << " ", decl->type);
      return out;
  }
  return out;
}
