#include <lartc/ast/declaration.hh>
#include <lartc/internal_errors.hh>

Declaration* Declaration::New(declaration_t kind) {
  return new Declaration {
    .kind = kind,
    .children = {},
    .name = "", 
    .parent = nullptr,
    .type = nullptr,
    .parameters = {},
    .body = nullptr
  };
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
    Statement::Delete(decl->body);
    delete decl;
    decl = nullptr;
  }
}

std::ostream& Declaration::Print(std::ostream& out, Declaration* decl, uint64_t tabulation) {
  tabulate(out, tabulation);
  bool first;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      out << "module";
      if (!decl->name.empty())
        out << " " << decl->name;
      out<< " {" << std::endl;
      for (Declaration* child : decl->children) {
        Declaration::Print(out, child, tabulation + 1);
        out << std::endl;
      }
      tabulate(out, tabulation);
      return out << "}";
    case declaration_t::FUNCTION_DECL:
      out << "fn " << decl->name << "(";
      first = true;
      for (std::pair<std::string, Type*> item : decl->parameters) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        Type::Print(out << item.first << ": ", item.second);
      }
      Type::Print(out << ") -> ", decl->type, 0);
      if (decl->body != nullptr) {
        out << " ";
        Statement::Print(out, decl->body, tabulation);
        return out << std::endl;
      } else {
        return out << ";";
      }
    case declaration_t::TYPE_DECL:
      out << "type";
      if (!decl->name.empty())
        out << " " << decl->name;
      Type::Print(out << " = ", decl->type);
      return out << ";";
  }
  return out;
}

std::string Declaration::QualifiedName(Declaration* decl) {
  if (decl->parent != nullptr) {
    return decl->name;
  } else {
    std::string parent_name = Declaration::QualifiedName(decl->parent);
    return parent_name + "::" + decl->name;
  }
}

std::ostream& Declaration::PrintShort(std::ostream& out, Declaration* decl) {
  bool first;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      out << "mod " << Declaration::QualifiedName(decl);
      break;
    case declaration_t::FUNCTION_DECL:
      out << "fn " << Declaration::QualifiedName(decl) << "(";
      first = true;
      for (std::pair<std::string, Type*> item : decl->parameters) {
        if (first) {
          first = false;
        } else {
          out << ", ";
        }
        out << item.first;
      }
      break;
    case declaration_t::TYPE_DECL:
      out << "type " << Declaration::QualifiedName(decl);
      break;
  }
  return out;
}
