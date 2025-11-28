#include <cassert>
#include <lartc/constants/check_constants.hh>
#include <lartc/external_errors.hh>
#include <lartc/internal_errors.hh>

std::pair<bool, Expression*> check_constants(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, TypeCache& type_cache, ConstantCache& constant_cache, Declaration* decl, Expression* expr) {
  Expression* result = nullptr;
  bool ok = true;
  (void)file_db;
  (void)symbol_cache;
  (void)type_cache;
  (void)constant_cache;
  (void)decl;

  switch (expr->kind) {
    case VANEXT_EXPR:
      {
        ok = false;
        throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
        result = Expression::Clone(expr);
        break;
      }
    case CALL_EXPR:
      {
        ok = false;
        throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
        result = Expression::Clone(expr);
        break;
      }
    case ARRAY_ACCESS_EXPR:
      {
        ok = false;
        throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
        result = Expression::Clone(expr);
        break;
      }
    case INTEGER_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case DOUBLE_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case BOOLEAN_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case NULLPTR_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case CHARACTER_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case STRING_EXPR:
      {
        result = Expression::Clone(expr);
        break;
      }
    case SIZEOF_EXPR:
      {
        result = Expression::New(INTEGER_EXPR);
        result->integer_literal = size_cache.compute_size_in_byte_of(symbol_cache, decl, expr->type);
        break;
      }
    case SYMBOL_EXPR:
      {
        Declaration* reference = symbol_cache.get_declaration(decl, expr->symbol);
        if (reference->kind == declaration_t::STATIC_VARIABLE_DECL) {
          if (constant_cache.staging[reference]) {
            throw_cyclic_dependency_between_static_variables_is_not_protected_by_usage_of_pointers(file_db, file_db.expression_points[expr], decl, reference);
            result = Expression::Clone(expr);
            ok = false;
          } else {
          if (!constant_cache.constants.contains(reference)) {
            constant_cache.staging[reference] = true;
            auto checked = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, reference, reference->value);
            ok &= checked.first;
            if (ok) {
              result = Expression::Clone(checked.second);
            } else {
              result = Expression::Clone(expr);
            }
            constant_cache.staging[reference] = false;
          } else {
            result = Expression::Clone(constant_cache.constants[reference]);
          }
        }
        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
    case BINARY_EXPR:
      {
        if (expr->operator_ == operator_t::ARR_OP || expr->operator_ == operator_t::DOT_OP) {
          throw_expression_is_used_in_constant_context_but_is_not_constant(file_db, file_db.expression_points[expr], decl);
          ok = false;
        }
        auto left = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->left);
        auto right = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->right);
        ok &= left.first;
        ok &= right.first;
        if (ok) {

        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
    case MONARY_EXPR:
      {
        break;
      }
    case CAST_EXPR:
      {
        auto checked = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->value);
        ok &= checked.first;
        if (ok) {
          result = Expression::Clone(checked.second);
        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
    case BITCAST_EXPR:
      {
        auto checked = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, expr->value);
        ok &= checked.first;
        if (ok) {
          result = Expression::Clone(checked.second);
        } else {
          result = Expression::Clone(expr);
        }
        break;
      }
  }
  assert (result != nullptr);
  return {ok, result};
}

bool check_constants(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, TypeCache& type_cache, ConstantCache& constant_cache, Declaration* decl) {
  bool declared_types_ok = true;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      for (auto child : decl->children) {
        declared_types_ok &= check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, child);
      }
      break;
    case declaration_t::STATIC_VARIABLE_DECL:
      if (!constant_cache.constants.contains(decl)) {
        constant_cache.staging[decl] = true;
        if (decl->value != nullptr) {
          auto checked = check_constants(file_db, symbol_cache, size_cache, type_cache, constant_cache, decl, decl->value);
          constant_cache.constants[decl] = checked.second;
          declared_types_ok &= checked.first;
        }
        constant_cache.staging[decl] = false;
      }
      break;
    case declaration_t::FUNCTION_DECL:
      break;
    case declaration_t::TYPE_DECL:
      break;
  }
  return declared_types_ok;
}
