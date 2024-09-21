#include <cstdlib>
#include <iostream>
#include <lartc/codegen/emit_llvm.hh>
#include <lartc/codegen/markers.hh>
#include <cassert>
#include <lartc/terminal.hh>
#include <lartc/serializations.hh>

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
  assert(decl != nullptr);
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
        // is it possible that in this scope this symbol was never searched because its owned by a borrowed definition
        Declaration* source = context.symbol_cache.get_or_find_declaration(decl, type->symbol);
        assert(source != nullptr);
        emit_decl_label(out << "%", source);
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
    out << _implicit_var << " = alloca ";
    emit_type_specifier(out, context, func, func->type);
    out << ", align 8" << std::endl;
    out << _implicit_rvalue << " = load ";
    emit_type_specifier(out, context, func, func->type);
    out << ", ptr " << _implicit_var << ", align 8" << std::endl;
    out << "ret ";
    emit_type_specifier(out, context, func, func->type);
    out << " " << _implicit_rvalue << std::endl;
  } else {
    out << "ret void" << std::endl;
  }
  return out;
}

std::ostream& emit_manual_return_statement(std::ostream& out, CGContext& context, Declaration* func, Markers& /*markers*/, const std::string& return_value_marker) {
  if (func->type->kind != VOID_TYPE) {
    out << "ret ";
    emit_type_specifier(out, context, func, func->type);
    out << " " << return_value_marker << std::endl;
  } else {
    out << "ret void" << std::endl;
  }
  return out;
}

std::ostream& emit_variable_allocation(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Statement* variable) {
  std::string var = markers.get_var(variable);
  out << var << " = alloca ";
  emit_type_specifier(out, context, func, variable->type);
  out << ", align 8" << std::endl;
  return out;
}

std::ostream& emit_expression_as_rvalue(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Expression* expression, std::string& output_marker) {
  out << "; " << expression->kind << std::endl;
  switch (expression->kind) {
    case SYMBOL_EXPR:
      {
        output_marker = markers.new_marker();
        out << output_marker << " = load ";
        if (Declaration* decl = context.symbol_cache.get_declaration(func, expression->symbol)) {
          emit_type_specifier(out, context, decl, decl->type);
          emit_decl_label(out << ", ptr ", decl) << ", align 8" << std::endl;
        } else if (Statement* var = context.symbol_cache.get_statement(expression)) {
          emit_type_specifier(out, context, func, var->type);
          out << ", ptr " << markers.get_var(var) << ", align 8" << std::endl;
        } else if (std::pair<std::string, Type*>* param = context.symbol_cache.get_parameter(expression)) {
          emit_type_specifier(out, context, func, param->second);
          out << ", ptr " << markers.get_param(param) << ", align 8" << std::endl;
        } else {
          assert(false);
        }
        break;
      }
    case INTEGER_EXPR:
      {
        output_marker = context.literal_store.get_int_literal(expression->integer_literal);
        break;
      }
    case DOUBLE_EXPR:
      {
        output_marker = context.literal_store.get_float_literal(expression->decimal_literal);
        break;
      }
    case BOOLEAN_EXPR:
      {
        output_marker = context.literal_store.get_int_literal(expression->boolean_literal);
        break;
      }
    case NULLPTR_EXPR:
      {
        output_marker = "null";
        break;
      }
    case CHARACTER_EXPR:
      {
        output_marker = context.literal_store.get_int_literal(expression->integer_literal);
        break;
      }
    case STRING_EXPR:
      {
        output_marker = context.literal_store.get_string_literal(expression->string_literal);
        break;
      }
    case CALL_EXPR:
      {
        // TODO:
        output_marker = markers.new_marker();
        break;
      }
    case BINARY_EXPR:
      {
        // TODO:
        output_marker = markers.new_marker();
        break;
      }
    case MONARY_EXPR:
      {
        // TODO:
        output_marker = markers.new_marker();
        break;
      }
    case SIZEOF_EXPR:
      {
        output_marker = context.literal_store.get_int_literal(context.size_cache.compute_size_of(context.symbol_cache, func, expression->type));
        break;
      }
    case CAST_EXPR: // TODO:
    case BITCAST_EXPR:
      {
        std::string value_marker;
        emit_expression_as_rvalue(out, context, func, markers, expression->value, value_marker);
        output_marker = markers.new_marker();
        out << output_marker << " = bitcast ";
        emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->value]);
        out << " " << value_marker << " to ";
        emit_type_specifier(out, context, func, expression->type);
        break;
      }
  }
  return out;
}

std::ostream& emit_expression_as_lvalue(std::ostream& out, CGContext& /*context*/, Declaration* /*func*/, Markers& markers, Expression* expression, std::string& output_marker) {
  out << "; " << expression->kind << std::endl;
  output_marker = markers.new_marker();
  switch (expression->kind) {
    case SYMBOL_EXPR:
      {
        break;
      }
    case BINARY_EXPR:
      {
        break;
      }
    case MONARY_EXPR:
      {
        break;
      }
    case INTEGER_EXPR:
    case DOUBLE_EXPR:
    case BOOLEAN_EXPR:
    case NULLPTR_EXPR:
    case CHARACTER_EXPR:
    case STRING_EXPR:
    case CALL_EXPR:
    case SIZEOF_EXPR:
    case CAST_EXPR:
    case BITCAST_EXPR:
      assert(false);
      break;
  }
  return out;
}

std::ostream& emit_statement(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Statement* statement) {
  out << "; " << statement->kind << std::endl;
  switch (statement->kind) {
    case statement_t::FOR_STMT:
      {
        PRESERVE_MARKER_KEY(CONTINUE_MK);
        PRESERVE_MARKER_KEY(BREAK_MK);

        std::string before_condition = markers.new_marker();
        std::string before_body = markers.new_marker();
        std::string after_body = markers.new_marker(CONTINUE_MK);
        std::string end_for = markers.new_marker(BREAK_MK);

        emit_statement(out, context, func, markers, statement->init);
        out << "br label " << before_condition << std::endl;

        emit_marker(out, before_condition) << std::endl;
        std::string rvalue_marker;
        emit_expression_as_rvalue(out, context, func, markers, statement->condition, rvalue_marker);
        out << "br i1 " << rvalue_marker << ", label " << before_body << ", label " << end_for << std::endl;

        emit_marker(out, before_body) << std::endl;
        emit_statement(out, context, func, markers, statement->body);
        out << "br label " << after_body << std::endl;

        emit_marker(out, after_body) << std::endl;
        emit_expression_as_rvalue(out, context, func, markers, statement->step, rvalue_marker);
        out << "br label " << end_for << std::endl;

        emit_marker(out, end_for) << std::endl;

        RESTORE_MARKER_KEY(CONTINUE_MK);
        RESTORE_MARKER_KEY(BREAK_MK);
        break;
      }
    case statement_t::LET_STMT:
      {
        markers.add_var(statement);
        emit_variable_allocation(out, context, func, markers, statement);
        if (statement->expr != nullptr) {
          // TODO: initialize with expr
          std::string rvalue_marker;
          emit_expression_as_rvalue(out, context, func, markers, statement->expr, rvalue_marker);
        }
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
        std::string rvalue_marker;
        emit_expression_as_rvalue(out, context, func, markers, statement->condition, rvalue_marker);
        out << "br i1 " << rvalue_marker << ", label " << before_body << ", label " << after_body << std::endl;

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
        std::string rvalue_marker;
        if (statement->expr != nullptr) {
          emit_expression_as_rvalue(out, context, func, markers, statement->expr, rvalue_marker);
          emit_manual_return_statement(out, context, func, markers, rvalue_marker);
        } else {
          // rvalue_marker is unused when return-type is Void
          // TODO: enforce return-type match with function return type in type checking
          emit_manual_return_statement(out, context, func, markers, rvalue_marker);
        }
        markers.new_marker(); // consuming a marker for the sake of idk
        break;
      }
    case statement_t::IF_ELSE_STMT:
      {
        if (statement->else_ != nullptr) {
          std::string before_then = markers.new_marker();
          std::string before_else = markers.new_marker();
          std::string after_else = markers.new_marker();

          std::string rvalue_marker;
          emit_expression_as_rvalue(out, context, func, markers, statement->condition, rvalue_marker);
          out << "br i1 " << rvalue_marker << ", label " << before_then << ", label " << before_else << std::endl;
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

          std::string rvalue_marker;
          emit_expression_as_rvalue(out, context, func, markers, statement->condition, rvalue_marker);
          out << "br i1 " << rvalue_marker << ", label " << before_then << ", label " << after_then << std::endl;
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
        std::string rvalue_marker;
        emit_expression_as_rvalue(out, context, func, markers, statement->expr, rvalue_marker);
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

std::ostream& emit_parameters(std::ostream& out, CGContext& context, Markers& markers, Declaration* func) {
  for (auto param : func->parameters) {
    markers.add_param(&param);
    std::string param_marker = markers.get_param(&param);
    
    out << param_marker << " = alloca ";
    emit_type_specifier(out, context, func, param.second);
    out << ", align 8" << std::endl;

    out << param_marker << " = load ";
    emit_type_specifier(out, context, func, param.second);
    out << ", ptr %" << param.first << ", align 8" << std::endl;
  }
  return out;
}

std::ostream& emit_function_definition(std::ostream& out, CGContext& context, Declaration* decl) {
  out << "define ";
  emit_type_specifier(out, context, decl, decl->type);
  out << " @";
  emit_decl_label(out, decl);
  out  << "(";
  bool first = true;
  for (auto param : decl->parameters) {
    if (first) {
      first = false;
    } else {
      out << ", ";
    }
    emit_type_specifier(out, context, decl, param.second);
    out << " %" << param.first;
  }
  out << ") {" << std::endl;
  Markers markers;
  emit_parameters(out, context, markers, decl);
  emit_statement(out, context, decl, markers, decl->body);
  emit_automatic_return_statement(out, context, decl, markers);
  out << "}" << std::endl;
  return out;
}

void emit_declaration(std::ostream& out, CGContext& context, Declaration* decl) {
  switch (decl->kind) {
    case MODULE_DECL:
      {
        for (Declaration* child : decl->children) {
          emit_declaration(out, context, child);
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

void emit_literal_store(std::ostream& out, CGContext& context) {
  for (auto item : context.literal_store.string_literals) {
    out << context.literal_store.serialize(item.second) << " = private constant [" << item.first.size() + 1 << " x i8] c" << dump_unescaped_string(item.first, true) << ", align 1" << std::endl;
  }
  for (auto item : context.literal_store.int_literals) {
    out << context.literal_store.serialize(item.second) << " = private constant i64 " << item.first << ", align 1" << std::endl;
  }
  for (auto item : context.literal_store.float_literals) {
    out << context.literal_store.serialize(item.second) << " = private constant double " << item.first << ", align 1" << std::endl;
  }
}

void emit_llvm(std::ostream& out, CGContext& context, Declaration* decl_tree) {
  emit_declaration(out, context, decl_tree);
  emit_literal_store(out, context);
}
