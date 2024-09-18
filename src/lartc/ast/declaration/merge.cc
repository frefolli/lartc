#include <lartc/ast/declaration/merge.hh>
#include <lartc/external_errors.hh>
#include <cassert>

bool type_matches_symbolically(Type* older, Type* latest) {
  if (older->kind != latest->kind)
    return false;

  switch (older->kind) {
    case type_t::INTEGER_TYPE:
      return older->size == latest->size && older->is_signed == latest->is_signed;
    case type_t::DOUBLE_TYPE:
      return older->size == latest->size;
    case type_t::BOOLEAN_TYPE:
      return true;
    case type_t::POINTER_TYPE:
      return type_matches_symbolically(older->subtype, latest->subtype);
    case type_t::SYMBOL_TYPE:
      {
        int64_t older_index = older->symbol.identifiers.size() - 1;
        int64_t latest_index = latest->symbol.identifiers.size() - 1;
        while (older_index > 0 && latest_index > 0) {
          if (older->symbol.identifiers.at(older_index) == latest->symbol.identifiers.at(latest_index)) {
            --older_index;
            --latest_index;
          } else {
            return false;
          }
        }
        // For now i assume it's true
        return true;
      }
    case type_t::VOID_TYPE:
      return true;
    case type_t::STRUCT_TYPE:
      {
        if (older->fields.size() != latest->fields.size())
          return false;
        uint64_t n_of_fields = older->fields.size();
        for (uint64_t index = 0; index < n_of_fields; ++index) {
          if (!type_matches_symbolically(older->fields[index].second, latest->fields[index].second)) {
            return false;
          }
        }
        return true;
      }
    case type_t::FUNCTION_TYPE:
      {
        if (older->parameters.size() != latest->parameters.size())
          return false;
        uint64_t n_of_parameters = older->parameters.size();
        for (uint64_t index = 0; index < n_of_parameters; ++index) {
          if (!type_matches_symbolically(older->parameters[index].second, latest->parameters[index].second)) {
            return false;
          }
        }
        return type_matches_symbolically(older->subtype, latest->subtype);
      }
  }
  return false;
}

Declaration* merge_type_declarations(TSContext& context, Declaration* older, Declaration* latest) {
  Declaration* result = nullptr;
  if (type_matches_symbolically(older->type, latest->type)) {
    context.file_db->declaration_points.erase(latest);
    Declaration::Delete(latest);
    result = older;
  } else {
    throw_duplicate_type_definition_doesnt_match(context.file_db->declaration_points[older], context.file_db->declaration_points[latest]);
    context.ok = false;
    
    context.file_db->declaration_points.erase(latest);
    Declaration::Delete(latest);
    result = older;
  }
  assert(result != nullptr);
  return result;
}

Declaration* merge_module_declarations(TSContext& context, Declaration* older, Declaration* latest) {
  Declaration* result = nullptr;

  while (latest->children.size() > 0) {
    Declaration* latest_child = latest->children.back();
    latest->children.pop_back();
    Declaration* older_child = older->find_child(latest_child->name);
    if (older_child != nullptr) {
      older->remove_child(older_child);
      latest_child = merge_declarations(context, older_child, latest_child);
    }
    older->children.push_back(latest_child);
  }

  context.file_db->declaration_points.erase(latest);
  Declaration::Delete(latest);
  result = older;

  assert(result != nullptr);
  return result;
}

Declaration* merge_function_declarations(TSContext& context, Declaration* older, Declaration* latest) {
  Declaration* result = nullptr;

  uint64_t n_of_parameters = older->parameters.size();
  if (n_of_parameters == latest->parameters.size()) {
    bool ok = true;
    for (uint64_t index = 0; index < n_of_parameters; ++index) {
      if (!type_matches_symbolically(older->parameters.at(index).second, latest->parameters.at(index).second)) {
        ok = false;
        break;
      }
    }

    if (ok) {
      if (type_matches_symbolically(older->type, latest->type)) {
        if (older->body != nullptr) {
          if (latest->body != nullptr) {
            // err: redefinition
            throw_duplicate_function_definition(context.file_db->declaration_points[older], context.file_db->declaration_points[latest]);
            context.ok = false;
          }
        } else {
          if (latest->body != nullptr) {
            older->body = latest->body;
            latest->body = nullptr;
          }
        }
      } else {
        throw_duplicate_function_declaration_return_type_doesnt_match(context.file_db->declaration_points[older], context.file_db->declaration_points[latest]);
        context.ok = false;
      }
    } else {
      throw_duplicate_function_declaration_parameter_types_dont_match(context.file_db->declaration_points[older], context.file_db->declaration_points[latest]);
      context.ok = false;
    }
  } else {
    throw_duplicate_function_declaration_wrong_parameter_number(context.file_db->declaration_points[older], context.file_db->declaration_points[latest]);
    context.ok = false;
  }

  context.file_db->declaration_points.erase(latest);
  Declaration::Delete(latest);
  result = older;

  assert(result != nullptr);
  return result;
}

Declaration* merge_declarations(TSContext& context, Declaration* older, Declaration* latest) {
  Declaration* result = nullptr;
  if (older->kind != latest->kind) {
    throw_duplicate_declaration_matches_name_but_not_kind(context.file_db->declaration_points[older], context.file_db->declaration_points[latest]);
    context.ok = false;

    context.file_db->declaration_points.erase(latest);
    Declaration::Delete(latest);
    result = older;
  } else {
    switch (older->kind) {
      case declaration_t::TYPE_DECL:
        {
          result = merge_type_declarations(context, older, latest);
          break;
        }
      case declaration_t::MODULE_DECL:
        {
          result = merge_module_declarations(context, older, latest);
          break;
        }
      case declaration_t::FUNCTION_DECL:
        {
          result = merge_function_declarations(context, older, latest);
          break;
        }
    }
  }
  assert(result != nullptr);
  return result;
}
