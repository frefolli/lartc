#include "lartc/ast/operator.hh"
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

std::string craft_decl_label(Declaration* decl) {
  assert(decl != nullptr);
  std::string result;
  if (decl->parent != nullptr && decl->parent->name != "") {
    result += craft_decl_label(decl->parent) + "__";
  }
  result += decl->name;
  return result;
}

std::ostream& emit_decl_label(std::ostream& out, Declaration* decl) {
  return out << craft_decl_label(decl);
}

Type* extract_subtype(CGContext& context, Declaration* decl, Type* type) {
  switch (type->kind) {
    case POINTER_TYPE:
      {
        return type->subtype;
      }
    case FUNCTION_TYPE:
      {
        return type->subtype;
      }
    case SYMBOL_TYPE:
      {
        Declaration* source = context.symbol_cache.get_or_find_declaration(decl, type->symbol);
        assert(source != nullptr);
        return extract_subtype(context, source, source->type);
      }
    default:
      assert(false);
  }
}

std::vector<std::pair<std::string, Type*>>* extract_parameters(CGContext& context, Declaration* decl, Type* type) {
  switch (type->kind) {
    case FUNCTION_TYPE:
      {
        return &type->parameters;
      }
    case SYMBOL_TYPE:
      {
        Declaration* source = context.symbol_cache.get_or_find_declaration(decl, type->symbol);
        assert(source != nullptr);
        return extract_parameters(context, source, source->type);
      }
    default:
      assert(false);
  }
}

std::vector<std::pair<std::string, Type*>>* extract_fields(CGContext& context, Declaration* decl, Type* type) {
  switch (type->kind) {
    case FUNCTION_TYPE:
      {
        return &type->fields;
      }
    case SYMBOL_TYPE:
      {
        Declaration* source = context.symbol_cache.get_or_find_declaration(decl, type->symbol);
        assert(source != nullptr);
        return extract_fields(context, source, source->type);
      }
    default:
      assert(false);
  }
}

bool type_is_pointer(CGContext& /*context*/, Declaration* /*decl*/, Type* type) {
  return type->kind == POINTER_TYPE;
}

bool type_is_integer(CGContext& /*context*/, Declaration* /*decl*/, Type* type) {
  return type->kind == INTEGER_TYPE;
}

bool type_is_double(CGContext& /*context*/, Declaration* /*decl*/, Type* type) {
  return type->kind == DOUBLE_TYPE;
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

std::ostream& cast_operands_to_expression_type(std::ostream& out, CGContext& context, Markers& markers, Declaration* func, Type* left_type, std::string& left_marker, Type* right_type, std::string& right_marker, Type* type) {
  uint64_t left_type_size = context.size_cache.compute_size_of(context.symbol_cache, func, left_type);
  uint64_t right_type_size = context.size_cache.compute_size_of(context.symbol_cache, func, right_type);
  uint64_t type_size = context.size_cache.compute_size_of(context.symbol_cache, func, type);
  if (left_type_size > type_size) {
    std::string output_marker = markers.new_marker();
    emit_type_specifier(emit_type_specifier(out << output_marker << " = trunc ", context, func, left_type) << " " << left_marker << " to ", context, func, right_type) << std::endl;
    left_marker = output_marker;
  } else if (left_type_size < type_size) {
    std::string output_marker = markers.new_marker();
    emit_type_specifier(emit_type_specifier(out << output_marker << " = zext ", context, func, left_type) << " " << left_marker << " to ", context, func, right_type) << std::endl;
    left_marker = output_marker;
  }
  if (right_type_size > type_size) {
    std::string output_marker = markers.new_marker();
    emit_type_specifier(emit_type_specifier(out << output_marker << " = trunc ", context, func, right_type) << " " << right_marker << " to ", context, func, right_type) << std::endl;
    right_marker = output_marker;
  } else if (right_type_size < type_size) {
    std::string output_marker = markers.new_marker();
    emit_type_specifier(emit_type_specifier(out << output_marker << " = zext ", context, func, right_type) << " " << right_marker << " to ", context, func, right_type) << std::endl;
    right_marker = output_marker;
  }
  return out;
}

Type* decide_logic_operand_type(CGContext& context, Declaration* decl, Type* left, Type* right) {
  Declaration* left_decl = decl;
  while (left->kind == SYMBOL_TYPE) {
    Declaration* source = context.symbol_cache.get_or_find_declaration(left_decl, left->symbol);
    assert(source != nullptr);
    left_decl = source;
    left = left_decl->type;
  }
  Declaration* right_decl = decl;
  while (right->kind == SYMBOL_TYPE) {
    Declaration* source = context.symbol_cache.get_or_find_declaration(right_decl, right->symbol);
    assert(source != nullptr);
    right_decl = source;
    right = right_decl->type;
  }

  Type* type = nullptr;
  if (left->kind == DOUBLE_TYPE || right->kind == DOUBLE_TYPE) {
    type = Type::New(DOUBLE_TYPE);
  } else if (left->kind == POINTER_TYPE || right->kind == POINTER_TYPE) {
    if (left->kind == POINTER_TYPE) {
      type = Type::Clone(left);
    } else {
      type = Type::Clone(right);
    }
  } else if (left->kind == INTEGER_TYPE || right->kind == INTEGER_TYPE) {
    if (left->kind  == INTEGER_TYPE) {
      type = Type::Clone(left);
    }
    if (right->kind  == INTEGER_TYPE) {
      if (type == nullptr) {
        type = Type::Clone(right);
      } else if (type->size < right->size) {
        type->size = right->size;
      }
    }
  } else if (left->kind == BOOLEAN_TYPE || right->kind == BOOLEAN_TYPE) {
    type = Type::New(BOOLEAN_TYPE);
  }
  assert(type != nullptr);
  return type;
}

std::ostream& cast_operands_to_biggest_type(std::ostream& out, CGContext& context, Markers& markers, Declaration* func, Type* left_type, std::string& left_marker, Type* right_type, std::string& right_marker) {
  return cast_operands_to_expression_type(out, context, markers, func, left_type, left_marker, right_type, right_marker, decide_logic_operand_type(context, func, left_type, right_type));
}

std::ostream& emit_simple_binary_operation(std::ostream& out, CGContext& context, Declaration* func, const std::string& output_marker, const std::string& left_marker, const std::string& right_marker, Type* type, std::string integer_op, std::string double_op) {
  if (type_is_integer(context, func, type)) {
    emit_type_specifier(out << output_marker << " = " << integer_op << " ", context, func, type) << " " << left_marker << ", " << right_marker << std::endl;
  } else if (type_is_double(context, func, type)) {
    emit_type_specifier(out << output_marker << " = " << double_op << " ", context, func, type) << " " << left_marker << ", " << right_marker << std::endl;
  }
  return out;
}

std::ostream& emit_integer_only_binary_operation(std::ostream& out, CGContext& context, Declaration* func, const std::string& output_marker, const std::string& left_marker, const std::string& right_marker, Type* type, std::string integer_op) {
  if (type_is_integer(context, func, type)) {
    emit_type_specifier(out << output_marker << " = " << integer_op << " ", context, func, type) << " " << left_marker << ", " << right_marker << std::endl;
  } else if (type_is_double(context, func, type)) {
    assert(false);
  }
  return out;
}

uint64_t compute_field_index(CGContext& context, Declaration* decl, Type* left_type, Expression* right) {
  Declaration* left_decl = decl;
  while (left_type->kind == SYMBOL_TYPE) {
    Declaration* source = context.symbol_cache.get_or_find_declaration(left_decl, left_type->symbol);
    assert(source != nullptr);
    left_decl = source;
    left_type = left_decl->type;
  }
  return Type::ExtractFieldIndex(left_type, right->symbol);
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

std::ostream& emit_expression_as_lvalue(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Expression* expression, std::string& output_marker);
std::ostream& emit_expression_as_rvalue(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Expression* expression, std::string& output_marker);

std::ostream& emit_expression_as_lvalue(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Expression* expression, std::string& output_marker) {
  // out << "; " << expression->kind << std::endl;
  switch (expression->kind) {
    case SYMBOL_EXPR:
      {
        if (Declaration* decl = context.symbol_cache.get_declaration(func, expression->symbol)) {
          output_marker = "@" + craft_decl_label(decl);
        } else if (Statement* var = context.symbol_cache.get_statement(expression)) {
          output_marker = markers.get_var(var);
          assert(!output_marker.empty());
        } else if (std::pair<std::string, Type*>* param = context.symbol_cache.get_parameter(expression)) {
          output_marker = markers.get_param(param);
          assert(!output_marker.empty());
        } else {
          assert(false);
        }
        break;
      }
    case BINARY_EXPR:
      {
        if (expression->operator_ == ARR_OP) {
          std::string left_value;
          emit_expression_as_rvalue(out, context, func, markers, expression->left, left_value);
          Type* left_type = extract_subtype(context, func, context.type_cache.expression_types[expression->left]);
          output_marker = markers.new_marker();

          emit_type_specifier(out << output_marker << " = getelementptr ", context, func, left_type) << ", ptr " << left_value;
          uint64_t field_index = compute_field_index(context, func, left_type, expression->right);
          out << ", i64 0, i32 " << field_index << std::endl;
        } else if (expression->operator_ == DOT_OP) {
          std::string left_value;
          emit_expression_as_lvalue(out, context, func, markers, expression->left, left_value);
          Type* left_type = context.type_cache.expression_types[expression->left];
          output_marker = markers.new_marker();

          emit_type_specifier(out << output_marker << " = getelementptr ", context, func, left_type) << ", ptr " << left_value;
          uint64_t field_index = compute_field_index(context, func, left_type, expression->right);
          out << ", i64 0, i32 " << field_index << std::endl;
        } else {
          assert(false);
        }
        break;
      }
    case MONARY_EXPR:
      {
        output_marker = "monary_expr";
        // output_marker = markers.new_marker();
        break;
      }
    case CALL_EXPR:
      {
        output_marker = "call_expr";
        // output_marker = markers.new_marker();
        break;
      }
    case BITCAST_EXPR:
    case CAST_EXPR:
      {
        output_marker = "cast_expr";
        // output_marker = markers.new_marker();
        break;
      }
    case INTEGER_EXPR:
    case DOUBLE_EXPR:
    case BOOLEAN_EXPR:
    case NULLPTR_EXPR:
    case CHARACTER_EXPR:
    case STRING_EXPR:
    case SIZEOF_EXPR:
      assert(false);
      break;
  }
  return out;
}

std::ostream& emit_expression_as_rvalue(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Expression* expression, std::string& output_marker) {
  // out << "; " << expression->kind << std::endl;
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
          std::string marker = markers.get_var(var);
          assert(!marker.empty());
          out << ", ptr " << marker << ", align 8" << std::endl;
        } else if (std::pair<std::string, Type*>* param = context.symbol_cache.get_parameter(expression)) {
          emit_type_specifier(out, context, func, param->second);
          std::string marker = markers.get_param(param);
          assert(!marker.empty());
          out << ", ptr " << marker << ", align 8" << std::endl;
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
        std::string callable_marker;
        emit_expression_as_lvalue(out, context, func, markers, expression->callable, callable_marker);
        Type* callable_type = context.type_cache.expression_types[expression->callable];

        std::vector<std::string> argument_markers = {};
        for (uint64_t arg_index = 0; arg_index < expression->arguments.size(); ++arg_index) {
          std::string argument_marker;
          emit_expression_as_rvalue(out, context, func, markers, expression->arguments[arg_index], argument_marker);
          argument_markers.push_back(argument_marker);
        }

        Type* callable_subtype = extract_subtype(context, func, callable_type);
        if (callable_subtype->kind == VOID_TYPE) {
          out << output_marker << "call void";
        } else {
          output_marker = markers.new_marker();
          out << output_marker << " = call ";
          emit_type_specifier(out, context, func, callable_subtype);
        }
        out << " " << callable_marker << "(";
        for (uint64_t arg_index = 0; arg_index < expression->arguments.size(); ++arg_index) {
          if (arg_index > 0) {
            out << ", ";
          }
          Type* arg_type = context.type_cache.expression_types[expression->arguments[arg_index]];
          emit_type_specifier(out, context, func, arg_type) << " " << argument_markers[arg_index];
        }
        out << ")" << std::endl;
        break;
      }
    case BINARY_EXPR:
      {
        if (expression->operator_ == ARR_OP) {
          std::string referenced;
          emit_expression_as_lvalue(out, context, func, markers, expression, referenced);
          Type* type = context.type_cache.expression_types[expression];
          output_marker = markers.new_marker();
          emit_type_specifier(out << output_marker << " = load ", context, func, type) << ", ptr " << referenced << ", align 8" << std::endl;
        } else if (expression->operator_ == DOT_OP) {
          std::string referenced;
          emit_expression_as_lvalue(out, context, func, markers, expression, referenced);
          Type* type = context.type_cache.expression_types[expression];
          output_marker = markers.new_marker();
          emit_type_specifier(out << output_marker << " = load ", context, func, type) << ", ptr " << referenced << ", align 8" << std::endl;
        } else if (expression->operator_ == ASS_OP) {
          std::string right_value;
          emit_expression_as_rvalue(out, context, func, markers, expression->right, right_value);
          Type* right_type = context.type_cache.expression_types[expression->right];
          std::string left_value;
          emit_expression_as_lvalue(out, context, func, markers, expression->left, left_value);
          Type* left_type = context.type_cache.expression_types[expression->left];
          output_marker = markers.new_marker();

          emit_type_specifier(out << "store ", context, func, right_type) << " " << right_value << ", ptr " << left_value << std::endl;
          emit_type_specifier(out << output_marker << " = load ", context, func, left_type) << ", ptr " << left_value << ", align 8" << std::endl;
        } else {
          std::string right_value;
          emit_expression_as_rvalue(out, context, func, markers, expression->right, right_value);
          std::string left_value;
          emit_expression_as_rvalue(out, context, func, markers, expression->left, left_value);
          output_marker = markers.new_marker();

          if (is_algebraic_operator(expression->operator_)) {
            cast_operands_to_expression_type(out, context, markers, func, context.type_cache.expression_types[expression->left], left_value, context.type_cache.expression_types[expression->right], right_value, context.type_cache.expression_types[expression]);
          } else if (is_logical_operator(expression->operator_)) {
            cast_operands_to_biggest_type(out, context, markers, func, context.type_cache.expression_types[expression->left], left_value, context.type_cache.expression_types[expression->right], right_value);
          } else {
            assert(false);
          }

          switch (expression->operator_) {
            case MUL_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "mul", "fmul");
                }
              }
              break;
            case DIV_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "udiv", "fdiv");
                }
                break;
              }
            case ADD_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  Type* subtype = extract_subtype(context, func, context.type_cache.expression_types[expression]);
                  if (type_is_pointer(context, func, context.type_cache.expression_types[expression->right])) {
                    emit_type_specifier(out << output_marker << " = getelementptr (", context, func, subtype) << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->right]);
                    out << " " << right_value << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->left]);
                    out << " " << left_value << ")" << std::endl;
                  } else {
                    emit_type_specifier(out << output_marker << " = getelementptr (", context, func, subtype) << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->left]);
                    out << " " << left_value << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->right]);
                    out << " " << right_value << ")" << std::endl;
                  }
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "add", "fadd");
                }
                break;
              }
            case SUB_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  Type* subtype = extract_subtype(context, func, context.type_cache.expression_types[expression]);
                  if (type_is_pointer(context, func, context.type_cache.expression_types[expression->right])) {
                    std::string inverted_offset = markers.new_marker();
                    out << inverted_offset << " = mul ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->left]);
                    out << " " << left_value << ", -1" << std::endl;

                    emit_type_specifier(out << output_marker << " = getelementptr (", context, func, subtype) << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->right]);
                    out << " " << right_value << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->left]);
                    out << " " << inverted_offset << ")" << std::endl;
                  } else {
                    std::string inverted_offset = markers.new_marker();
                    out << inverted_offset << " = mul ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->right]);
                    out << " " << right_value << ", -1" << std::endl;

                    emit_type_specifier(out << output_marker << " = getelementptr (", context, func, subtype) << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->left]);
                    out << " " << left_value << ", ";
                    emit_type_specifier(out, context, func, context.type_cache.expression_types[expression->right]);
                    out << " " << right_value << ")" << std::endl;
                  }
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "sub", "fsub");
                }
                break;
              }
            case XOR_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_integer_only_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "xor");
                }
                break;
              }
            case AND_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_integer_only_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "and");
                }
                break;
              }
            case OR_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_integer_only_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "or");
                }
                break;
              }
            case LROT_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_integer_only_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "shl");
                }
                break;
              }
            case RROT_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_integer_only_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "lshr");
                }
                break;
              }
            case SCA_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_integer_only_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "and");
                }
                break;
              }
            case SCO_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_integer_only_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "or");
                }
                break;
              }
            case GE_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "icmp sge", "fcmp sge");
                }
                break;
              }
            case LE_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "icmp sle", "fcmp sle");
                }
                break;
              }
            case EQ_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "icmp eq", "fcmp eq");
                }
                break;
              }
            case NE_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "icmp ne", "fcmp ne");
                }
                break;
              }
            case GR_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "icmp sgt", "fcmp sgt");
                }
                break;
              }
            case LR_OP:
              {
                if (type_is_pointer(context, func, context.type_cache.expression_types[expression])) {
                  assert(false);
                } else {
                  emit_simple_binary_operation(out, context, func, output_marker, left_value, right_value, context.type_cache.expression_types[expression], "icmp slt", "fcmp slt");
                }
                break;
              }
            default:
              assert(false);
          }
        }
        break;
      }
    case MONARY_EXPR:
      {
        // TODO:
        switch (expression->operator_) {
          case MUL_OP: //*
            {
              std::string value_marker;
              emit_expression_as_rvalue(out, context, func, markers, expression->value, value_marker);
              output_marker = markers.new_marker();
              out << output_marker << " = load ";
              emit_type_specifier(out, context, func, extract_subtype(context, func, context.type_cache.expression_types[expression->value]));
              out << ", ptr " << value_marker << std::endl;
              break;
            }
          case AND_OP: //&
            {
              std::string value_marker;
              emit_expression_as_lvalue(out, context, func, markers, expression->value, value_marker);
              output_marker = value_marker;
              break;
            }
          default:
            {
              if (is_algebraic_operator(expression->operator_)) {
                std::string value_marker;
                emit_expression_as_rvalue(out, context, func, markers, expression->value, value_marker);
                output_marker = markers.new_marker();

                // TODO: STUB
                out << output_marker << " = fneg ";
                emit_type_specifier(out, context, func, context.type_cache.expression_types[expression]);
                out << " " << value_marker << std::endl;
              } else if (is_logical_operator(expression->operator_)) {
                std::string value_marker;
                emit_expression_as_rvalue(out, context, func, markers, expression->value, value_marker);
                output_marker = markers.new_marker();

                // TODO: STUB
                out << output_marker << " = fneg ";
                emit_type_specifier(out, context, func, context.type_cache.expression_types[expression]);
                out << " " << value_marker << std::endl;
              } else {
                assert(false);
              }
            }
        }
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

std::ostream& emit_statement(std::ostream& out, CGContext& context, Declaration* func, Markers& markers, Statement* statement) {
  // out << "; " << statement->kind << std::endl;
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
  for (uint64_t param_index = 0; param_index < func->parameters.size(); ++param_index) {
    std::pair<std::string, Type*>* param = func->parameters.data() + param_index;
    markers.add_param(param);
    std::string param_marker = markers.get_param(param);

    out << param_marker << " = alloca ";
    emit_type_specifier(out, context, func, param->second);
    out << ", align 8" << std::endl;

    out << "store ";
    emit_type_specifier(out, context, func, param->second);
    out << " %" << param->first << ", ptr " << param_marker << std::endl;
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
