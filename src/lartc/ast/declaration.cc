#include <cassert>
#include <lartc/ast/declaration.hh>
#include <lartc/internal_errors.hh>
#include <algorithm>

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

std::ostream& Declaration::Print(std::ostream& out, const Declaration* decl, uint64_t tabulation) {
  assert(decl != nullptr);
  tabulate(out, tabulation);
  bool first;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      if (!decl->name.empty()) {
        out << "mod";
        out << " " << decl->name;
        out<< " {" << std::endl;
        tabulation += 1;
      }
      for (Declaration* child : decl->children) {
        Declaration::Print(out, child, tabulation);
        out << std::endl;
      }
      if (!decl->name.empty()) {
        tabulate(out, tabulation - 1);
        out << "}";
      }
      return out;
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
        return out;
      } else {
        return out << ";";
      }
    case declaration_t::TYPE_DECL:
      out << "typedef";
      if (!decl->name.empty())
        out << " " << decl->name;
      Type::Print(out << " = ", decl->type);
      return out << ";";
    default:
      assert(false);
  }
  return out;
}

std::string Declaration::QualifiedName(const Declaration* decl) {
  if (decl->parent == nullptr) {
    return decl->name;
  } else {
    std::string parent_name = Declaration::QualifiedName(decl->parent);
    if (parent_name.size() > 0) {
      return parent_name + "::" + decl->name;
    }
    return decl->name;
  }
}

std::ostream& Declaration::PrintShort(std::ostream& out, const Declaration* decl) {
  assert(decl != nullptr);
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
      out << ")";
      break;
    case declaration_t::TYPE_DECL:
      out << "type " << Declaration::QualifiedName(decl);
      break;
  }
  return out;
}

Declaration* Declaration::find_child(const std::string& name) const {
  for (Declaration* child : children) {
    if (child->name == name) {
      return child;
    }
  }
  return nullptr;
}

void Declaration::remove_child(const Declaration* target) {
  auto it = std::find(children.begin(), children.end(), target);
  if (it != children.end()) {
    children.erase(it);
  }
}
