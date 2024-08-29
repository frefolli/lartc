#include <lartc/ast/declaration.hh>
#include <lartc/internal_errors.hh>

Declaration* Declaration::New(declaration_t kind) {
  return new Declaration {kind, {}, "", nullptr, nullptr};
}

void Declaration::Delete(Declaration*& decl) {
  if (decl != nullptr) {
    for (Declaration*& child : decl->children) {
      Declaration::Delete(child);
    }
    decl->children.clear();
    decl->name.clear();
    delete decl;
    decl = nullptr;
  }
}

std::ostream& Declaration::Print(std::ostream& out, Declaration* decl, uint64_t tabulation) {
  tabulate(out, tabulation) << decl->kind << " " << decl->name;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      out << " {" << std::endl;
      for (Declaration* child : decl->children) {
        Declaration::Print(out, child, tabulation + 1);
        out << std::endl;
      }
      return out << "}";
    case declaration_t::FUNCTION_DECL:
      out << " of type {" << std::endl;
      Type::Print(out, decl->type, tabulation + 1);
      return out << std::endl << "}";
    case declaration_t::TYPE_DECL:
      out << " of type {" << std::endl;
      Type::Print(out, decl->type, tabulation + 1);
      return out << std::endl << "}";
  }
  return out;
}
