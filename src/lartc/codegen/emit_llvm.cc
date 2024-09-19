#include <lartc/codegen/emit_llvm.hh>
#include <lartc/codegen/markers.hh>

std::ostream& emit_decl_label(std::ostream& out, Declaration* decl) {
  if (decl->parent != nullptr && decl->parent->name != "") {
    emit_decl_label(out, decl->parent) << "__";
  }
  return out << decl->name;
}

std::ostream& emit_type_specifier(std::ostream& out, CGContext& context, Declaration* decl, Type* type, bool first_level = true) {
  switch (type->kind) {
    case INTEGER_TYPE:
      {
        out << "i" << type->size;
        break;
      }
    case DOUBLE_TYPE:
      {
        if (type->size <= 32) {
          out << "float";
        } else if (type->size <= 64) {
          out << "double";
        } else {
          out << "fp128";
        }
        break;
      }
    case BOOLEAN_TYPE:
      {
        out << "i1";
        break;
      }
    case POINTER_TYPE:
      {
        emit_type_specifier(out, context, decl, type->subtype, false) << "*";
        break;
      }
    case SYMBOL_TYPE:
      {
        // resolve
        emit_decl_label(out << "%", context.symbol_cache.get_declaration(decl, type->symbol));
        break;
      }
    case VOID_TYPE:
      {
        if (first_level) {
          out << "void";
        } else {
          out << "i1";
        }
        break;
      }
    case STRUCT_TYPE:
      {
        out << "{";
        bool first = true;
        for (auto field : type->fields) {
          if (first) {
            first = false;
          } else {
            out << ", ";
          }
          emit_type_specifier(out, context, decl, field.second, false);
        }
        out << "}";
        break;
      }
    case FUNCTION_TYPE:
      {
        emit_type_specifier(out, context, decl, type->subtype) << " (";
        bool first = true;
        for (auto field : type->parameters) {
          if (first) {
            first = false;
          } else {
            out << ", ";
          }
          emit_type_specifier(out, context, decl, field.second, false);
        }
        out << ")";
        break;
      }
  }
  return out;
}

std::ostream& emit_automatic_return_statement(std::ostream& out, CGContext& context, Declaration* func, Markers& markers) {
  if (func->type->kind != VOID_TYPE) {
    std::string _implicit_var = markers.new_marker();
    std::string _implicit_rvalue = markers.new_marker();
    out << "  " << _implicit_var << " = alloca ";
    emit_type_specifier(out, context, func, func->type);
    out << ", align 8" << std::endl;
    out << "  " << _implicit_rvalue << " = load ";
    emit_type_specifier(out, context, func, func->type);
    out << ", ptr " << _implicit_var << ", align 8" << std::endl;
    out << "  ret ";
    emit_type_specifier(out, context, func, func->type);
    out << " " << _implicit_rvalue << std::endl;
  } else {
    out << "  ret void" << std::endl;
  }
  return out;
}

std::ostream& emit_function_declaration(std::ostream& out, CGContext& context, Declaration* decl) {
  out << "declare ";
  emit_type_specifier(out, context, decl, decl->type);
  out << " @";
  emit_decl_label(out, decl);
  out  << "(";
  bool first = true;
  for (auto field : decl->parameters) {
    if (first) {
      first = false;
    } else {
      out << ", ";
    }
    emit_type_specifier(out, context, decl, field.second);
  }
  out << ")" << std::endl;
  return out;
}

std::ostream& emit_function_definition(std::ostream& out, CGContext& context, Declaration* decl) {
  out << "define ";
  emit_type_specifier(out, context, decl, decl->type);
  out << " @";
  emit_decl_label(out, decl);
  out  << "(";
  bool first = true;
  for (auto field : decl->parameters) {
    if (first) {
      first = false;
    } else {
      out << ", ";
    }
    emit_type_specifier(out, context, decl, field.second);
  }
  out << ") {" << std::endl;
  Markers markers;
  emit_automatic_return_statement(out, context, decl, markers);
  out << "}" << std::endl;
  return out;
}

void emit_llvm(std::ostream& out, CGContext& context, Declaration* decl) {
  switch (decl->kind) {
    case MODULE_DECL:
    {
      for (Declaration* child : decl->children) {
        emit_llvm(out, context, child);
      }
      break;
    };
    case TYPE_DECL:
    {
      emit_type_specifier(emit_decl_label(out << "%", decl) << " = type ", context, decl, decl->type) << std::endl;
      break;
    };
    case FUNCTION_DECL:
    {
      if (decl->body == nullptr) {
        emit_function_declaration(out, context, decl);
      } else {
        emit_function_definition(out, context, decl);
      }
      break;
    };
  }
}
