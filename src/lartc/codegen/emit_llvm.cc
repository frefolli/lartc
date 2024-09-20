#include <lartc/codegen/emit_llvm.hh>
#include <lartc/codegen/markers.hh>
#include <cassert>

#define PRESERVE_MARKER_KEY(KEY) \
  int64_t preserved_##KEY = markers.save_key(KEY);

#define RESTORE_MARKER_KEY(KEY) \
  if (preserved_##KEY != 1) { \
    markers.restore_key(KEY, preserved_##KEY); \
  } else { \
    markers.no_key(KEY); \
  }

std::ostream& emit_marker(std::ostream& out, const std::string& marker) {
  // if marker is "%nn" i want to print "nn:"
  return out << marker.substr(1) << ':';
}

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

std::ostream& emit_variable_allocation(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Statement* variable) {
    std::string var = markers.get_var(variable);
    out << "  " << var << " = alloca ";
    emit_type_specifier(out, context, func, func->type);
    out << ", align 8" << std::endl;
  return out;
}

std::ostream& emit_statement(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Statement* statement) {
  out << "; " << statement->kind << std::endl;
  switch (statement->kind) {
    case statement_t::FOR_STMT:
      {
        PRESERVE_MARKER_KEY(CONTINUE_MK);
        PRESERVE_MARKER_KEY(BREAK_MK);

        /*
         * -- INIT --
         * BEFORE_CONDITION
         * -- CONDITION --
         * BEFORE_BODY
         * -- BODY --
         * AFTER_BODY (CONTINUE)
         * -- STEP --
         * END_FOR (BREAK)
         * */
        
        RESTORE_MARKER_KEY(CONTINUE_MK);
        RESTORE_MARKER_KEY(BREAK_MK);
        break;
      }
    case statement_t::LET_STMT:
      {
        markers.add_var(statement);
        emit_variable_allocation(out, context, func, markers, statement);
        // TODO: initialize with expr
        break;
      }
    case statement_t::BLOCK_STMT:
      {
        for (Statement* child : statement->children) {
          emit_statement(out, context, func, markers, child);
        }
        break;
      }
    case statement_t::BREAK_STMT:
      {
        std::string break_marker = markers.get_key(BREAK_MK);
        assert(!break_marker.empty());
        out << "br label " << break_marker << std::endl;
        break;
      }
    case statement_t::WHILE_STMT:
      {
        PRESERVE_MARKER_KEY(CONTINUE_MK);
        PRESERVE_MARKER_KEY(BREAK_MK);

        std::string before_condition = markers.new_marker(CONTINUE_MK);
        std::string before_body = markers.new_marker();
        std::string after_body = markers.new_marker(BREAK_MK);

        out << "br label " << before_condition << std::endl;
        emit_marker(out, before_condition) << std::endl;
        // TODO: compute condition
        // out << "br label " << after_body << std::endl;
        out << "br label " << before_body << std::endl;
        //
        emit_marker(out, before_body) << std::endl;
        emit_statement(out, context, func, markers, statement->body);
        out << "br label " << before_condition << std::endl;
        emit_marker(out, after_body) << std::endl;

        
        RESTORE_MARKER_KEY(CONTINUE_MK);
        RESTORE_MARKER_KEY(BREAK_MK);
        break;
      }
    case statement_t::RETURN_STMT:
      {
        // TODO:
        emit_automatic_return_statement(out, context, func, markers);
        break;
      }
    case statement_t::IF_ELSE_STMT:
      {
        if (statement->else_ != nullptr) {
          std::string before_then = markers.new_marker();
          std::string before_else = markers.new_marker();
          std::string after_else = markers.new_marker();

          // TODO: compute condition
          out << "br label " << before_then << std::endl;
          // out << "br label " << before_else << std::endl;
          emit_marker(out, before_then) << std::endl;

          emit_statement(out, context, func, markers, statement->then);
          out << "br label " << after_else << std::endl;

          emit_marker(out, before_else) << std::endl;

          emit_statement(out, context, func, markers, statement->else_);
          out << "br label " << after_else << std::endl;
          emit_marker(out, after_else) << std::endl;
        } else {
          std::string before_then = markers.new_marker();
          std::string after_then = markers.new_marker();

          // TODO: compute condition
          out << "br label " << before_then << std::endl;
          // out << "br label " << after_then << std::endl;
          emit_marker(out, before_then) << std::endl;

          emit_statement(out, context, func, markers, statement->then);
          out << "br label " << after_then << std::endl;

          emit_marker(out, after_then) << std::endl;
        }
        
        break;
      }
    case statement_t::CONTINUE_STMT:
      {
        std::string continue_marker = markers.get_key(CONTINUE_MK);
        assert(!continue_marker.empty());
        out << "br label " << continue_marker << std::endl;
        break;
      }
    case statement_t::EXPRESSION_STMT:
      {
        break;
      }
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
  emit_statement(out, context, decl, markers, decl->body);
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
