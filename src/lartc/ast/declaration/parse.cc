#include <lartc/ast/declaration/parse.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/ast/statement/parse.hh>
#include <lartc/ast/parse.hh>
#include <lartc/internal_errors.hh>
#include <lartc/tree_sitter.hh>
#include <cstring>
#include <tree_sitter/api.h>
#include <unordered_map>

inline void parse_declaration_module_rest(Declaration* decl, const TSLanguage* language, const char* source_code, TSNode& node, uint64_t from_index = 0) {
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = from_index; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    Declaration* child_decl = parse_declaration(language, source_code, child_node);
    if (child_decl != nullptr) {
      child_decl->parent = decl;
      decl->children.push_back(child_decl);
    } else {
      const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(child_node));
      if (!ts_can_ignore(symbol_name)) {
        throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (module)"));
      }
    }
  }
}

inline Declaration* parse_declaration_module(const TSLanguage* language, const char* source_code, TSNode& node) {
  Declaration* decl = Declaration::New(declaration_t::MODULE_DECL);
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  decl->name = ts_node_source_code(name, source_code);

  parse_declaration_module_rest(decl, language, source_code, node, 1);

  return decl;
}

Declaration* parse_source_file(const TSLanguage* language, const char* source_code, TSNode& root_node) {
  Declaration* source_file = Declaration::New(declaration_t::MODULE_DECL);
  const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(root_node));
  if (std::strcmp(symbol_name, "source_file") != 0)
    throw_internal_error(TS_ROOT_NODE_SHOULD_BE_SOURCE_FILE, MSG(": instead is " << std::string(symbol_name)));

  parse_declaration_module_rest(source_file, language, source_code, root_node, 0);

  return source_file;
}

inline Declaration* parse_declaration_function(const TSLanguage* language, const char* source_code, TSNode& node) {
  Declaration* decl = Declaration::New(declaration_t::FUNCTION_DECL);
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  decl->name = ts_node_source_code(name, source_code);
  
  TSNode parameters = ts_node_child_by_field_name(node, "parameters");
  decl->parameters = parse_field_parameter_list(language, source_code, parameters);

  TSNode type = ts_node_child_by_field_name(node, "type");
  if (type.id) {
    decl->type = parse_type(language, source_code, type);
  } else {
    decl->type = Type::New(type_t::VOID_TYPE);
  }
  
  TSNode body = ts_node_child_by_field_name(node, "body");
  if (body.id) {
    decl->body = parse_statement(language, source_code, body);
  } else {
    decl->body = nullptr;
  }

  if (decl->type == nullptr) {
    const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(type));
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (function)"));
  }
  
  return decl;
}

inline Declaration* parse_declaration_type(const TSLanguage* language, const char* source_code, TSNode& node) {
  Declaration* decl = Declaration::New(declaration_t::TYPE_DECL);
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  decl->name = ts_node_source_code(name, source_code);

  TSNode type = ts_node_child_by_field_name(node, "type");
  decl->type = parse_type(language, source_code, type);

  if (decl->type == nullptr) {
    const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(type));
    throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (typedef)"));
  }

  return decl;
}

typedef Declaration*(*declaration_parser)(const TSLanguage* language, const char* source_code, TSNode& node);
std::unordered_map<std::string, declaration_parser> declaration_parsers = {
  {"module", parse_declaration_module},
  {"function", parse_declaration_function},
  {"typedef", parse_declaration_type}
};

Declaration* parse_declaration(const TSLanguage* language, const char* source_code, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
  auto it = declaration_parsers.find(symbol_name);
  if (it != declaration_parsers.end()) {
    return it->second(language, source_code, node);
  }
  return nullptr;
}
