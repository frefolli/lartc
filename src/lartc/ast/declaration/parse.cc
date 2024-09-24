#include <algorithm>
#include <cassert>
#include <filesystem>
#include <lartc/ast/declaration/parse.hh>
#include <lartc/ast/declaration/merge.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/ast/statement/parse.hh>
#include <lartc/ast/parse.hh>
#include <lartc/internal_errors.hh>
#include <lartc/tree_sitter.hh>
#include <lartc/external_errors.hh>
#include <cstring>
#include <tree_sitter/api.h>
#include <unordered_map>

bool already_visited_or_in_queue(TSContext& context, const std::string& filepath) {
  if (std::find_if(context.file_db->files.begin(), context.file_db->files.end(), [&filepath](const FileDB::File& file) {
    return file.filepath == filepath;
  }) != context.file_db->files.end()) {
    return true;
  }

  if (std::find(context.file_queue.begin(), context.file_queue.end(), filepath) != context.file_queue.end()) {
    return true;
  }

  return false;
}

void append_filepath_or_throw(TSContext& context, TSNode& include_node, std::string& filepath, std::string& filepath_raw) {
  if (filepath == "") {
    throw_unable_to_resolve_include_filepath(*context.file_db, FileDB::Point::From(context.file_db, include_node), filepath_raw);
    context.ok = false;
  } else {
    if (!already_visited_or_in_queue(context, filepath)) {
      context.file_queue.push_back(filepath);
    }
  }
}

void parse_include_directive(TSContext& context, TSNode& include_node) {
  TSNode localpath_field = ts_node_child_by_field_name(include_node, "localpath");
  TSNode globalpath_field = ts_node_child_by_field_name(include_node, "globalpath");
  
  if (localpath_field.id != nullptr) {
    std::string localpath_raw = ts_node_source_code(localpath_field, context.source_code);
    std::string localpath = FileDB::resolve_local(localpath_raw, std::filesystem::canonical(context.filepath));
    append_filepath_or_throw(context, include_node, localpath, localpath_raw);
  } else if (globalpath_field.id != nullptr) {
    std::string globalpath_raw = ts_node_source_code(globalpath_field, context.source_code);
    std::string globalpath = FileDB::resolve_global(globalpath_raw);
    append_filepath_or_throw(context, include_node, globalpath, globalpath_raw);
  } else {
    throw_internal_error(INCLUDE_DIRECTORY_SHOULD_HAVE_LOCAL_OR_GLOBAL_PATH, MSG(""));
  }
}

inline void parse_declaration_module_rest(Declaration* decl, TSContext& context, TSNode& node, uint64_t from_index = 0) {
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = from_index; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(child_node));
    if (strcmp("include", symbol_name) == 0) {
      parse_include_directive(context, child_node);
    } else {
      Declaration* child_decl = parse_declaration(context, child_node);
      if (child_decl != nullptr) {
        const std::string& decl_name = child_decl->name;
        Declaration* older_decl = decl->find_child(decl_name);
        if (older_decl != nullptr) {
          decl->remove_child(older_decl);
          child_decl = merge_declarations(context, older_decl, child_decl);
        }

        child_decl->parent = decl;
        decl->children.push_back(child_decl);
      } else {
        if (!ts_can_ignore(symbol_name)) {
          throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (module)"));
        }
      }
    }
  }
}

inline Declaration* parse_declaration_module(TSContext& context, TSNode& node) {
  Declaration* decl = Declaration::New(declaration_t::MODULE_DECL);
  context.file_db->add_declaration(decl, node);
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  decl->name = ts_node_source_code(name, context.source_code);

  parse_declaration_module_rest(decl, context, node, 1);

  return decl;
}

void parse_source_file(Declaration* decl_tree, TSContext& context, TSNode& root_node) {
  const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(root_node));
  if (std::strcmp(symbol_name, "source_file") != 0)
    throw_internal_error(TS_ROOT_NODE_SHOULD_BE_SOURCE_FILE, MSG(": instead is " << std::string(symbol_name)));
  parse_declaration_module_rest(decl_tree, context, root_node, 0);
}

inline Declaration* parse_declaration_function(TSContext& context, TSNode& node) {
  Declaration* decl = Declaration::New(declaration_t::FUNCTION_DECL);
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  decl->name = ts_node_source_code(name, context.source_code);
  
  TSNode parameters = ts_node_child_by_field_name(node, "parameters");
  decl->parameters = parse_field_parameter_list(context, parameters);

  TSNode type = ts_node_child_by_field_name(node, "type");
  if (type.id) {
    decl->type = parse_type(context, type);
  } else {
    decl->type = Type::New(type_t::VOID_TYPE);
  }
  
  TSNode body = ts_node_child_by_field_name(node, "body");
  if (body.id) {
    decl->body = parse_statement(context, body);
  } else {
    decl->body = nullptr;
  }

  if (decl->type == nullptr) {
    const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(type));
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (function)"));
  }
  
  context.file_db->add_declaration(decl, node);
  return decl;
}

inline Declaration* parse_declaration_type(TSContext& context, TSNode& node) {
  Declaration* decl = Declaration::New(declaration_t::TYPE_DECL);
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  decl->name = ts_node_source_code(name, context.source_code);

  TSNode type = ts_node_child_by_field_name(node, "type");
  decl->type = parse_type(context, type);

  if (decl->type == nullptr) {
    const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(type));
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (typedef)"));
  }

  context.file_db->add_declaration(decl, node);
  return decl;
}

typedef Declaration*(*declaration_parser)(TSContext& context, TSNode& node);
std::unordered_map<std::string, declaration_parser> declaration_parsers = {
  {"module", parse_declaration_module},
  {"function", parse_declaration_function},
  {"typedef", parse_declaration_type},
};

Declaration* parse_declaration(TSContext& context, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(node));
  auto it = declaration_parsers.find(symbol_name);
  if (it != declaration_parsers.end()) {
    return it->second(context, node);
  }
  return nullptr;
}
