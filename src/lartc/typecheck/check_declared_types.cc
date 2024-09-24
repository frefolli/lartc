#include <lartc/typecheck/check_declared_types.hh>
#include <lartc/external_errors.hh>
#include <lartc/api/config.hh>

std::pair<bool, uint64_t> check_declared_types(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, Declaration* context, Type* type) {
  bool declared_types_ok = true;
  uint64_t size = 0;
  switch (type->kind) {
    case type_t::POINTER_TYPE:
      size = API::POINTER_SIZE;
      break;
    case type_t::VOID_TYPE:
      break;
    case type_t::DOUBLE_TYPE:
      size = type->size;
      break;
    case type_t::INTEGER_TYPE:
      size = type->size;
      break;
    case type_t::STRUCT_TYPE:
      for (auto item : type->fields) {
        auto checked = check_declared_types(file_db, symbol_cache, size_cache, context, item.second);
        declared_types_ok &= checked.first;
        size += checked.second;
      }
      break;
    case type_t::SYMBOL_TYPE:
      {
        Declaration* decl = symbol_cache.get_declaration(context, type->symbol);
        if (decl->kind != declaration_t::TYPE_DECL) {
          throw_a_type_definition_cannot_reference_a_non_type_declaration(file_db.symbol_points[&type->symbol], context, decl);
          declared_types_ok = false;
        } else if (size_cache.staging[decl]) {
          throw_cyclic_dependency_between_types_is_not_protected_by_usage_of_pointers(file_db.symbol_points[&type->symbol], context, decl);
          declared_types_ok = false;
        } else {
          if (!size_cache.sizes.contains(decl)) {
            size_cache.staging[decl] = true;
            auto checked = check_declared_types(file_db, symbol_cache, size_cache, decl, decl->type);
            declared_types_ok &= checked.first;
            size = checked.second;
            size_cache.staging[decl] = false;
          }
          size += size_cache.sizes[decl];
        }
      }
      break;
    case type_t::BOOLEAN_TYPE:
      size = 1;
      break;
    case type_t::FUNCTION_TYPE:
      size = API::POINTER_SIZE;
      break;
  }
  return {declared_types_ok, size};
}

bool check_declared_types(FileDB& file_db, SymbolCache& symbol_cache, SizeCache& size_cache, Declaration* decl) {
  bool declared_types_ok = true;
  switch (decl->kind) {
    case declaration_t::MODULE_DECL:
      for (auto child : decl->children) {
        declared_types_ok &= check_declared_types(file_db, symbol_cache, size_cache, child);
      }
      break;
    case declaration_t::TYPE_DECL:
      if (!size_cache.sizes.contains(decl)) {
        size_cache.staging[decl] = true;
        auto checked = check_declared_types(file_db, symbol_cache, size_cache, decl, decl->type);
        size_cache.sizes[decl] = checked.second;
        declared_types_ok &= checked.first;
        size_cache.staging[decl] = false;
      }
      break;
    case declaration_t::FUNCTION_DECL:
      break;
  }
  return declared_types_ok;
}
