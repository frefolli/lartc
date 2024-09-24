#include <cstddef>
#include <lartc/api/cpp.hh>
#include <lartc/terminal.hh>
#include <lartc/tree_sitter.hh>

#include <cstdint>
#include <iostream>
#include <string>
#include <tree_sitter/api.h>
#include <cstring>
#include <unordered_map>
#include <cassert>

extern "C" const TSLanguage *tree_sitter_c(void);

inline char* read_source_code(const char* filepath) {
    char* text = NULL;
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = (char*) malloc(fsize + 1);
    fread(text, fsize, 1, file);
    fclose(file);
    return text;
}

void crash_on_node(TSNode& node, const char* source_code, const std::string& symbol_name, const std::string& msg) {
  std::cerr << RED_TEXT << msg << NORMAL_TEXT << ": (" << symbol_name << ")" << std::endl;
  std::cerr << AZURE_TEXT << ts_node_string(node) << NORMAL_TEXT << std::endl;
  std::cerr << PURPLE_TEXT << ts_node_source_code(node, source_code) << NORMAL_TEXT << std::endl;
  std::exit(1);
}

void crash_on_line(const std::string& line, const std::string& msg) {
  std::cerr << RED_TEXT << msg << NORMAL_TEXT << ": '" << line << "'" << std::endl;
  std::exit(1);
}

void explore(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node);

std::string remove_piece(const std::string& line, const char* piece) {
  uint64_t len = strlen(piece);
  uint64_t i = len;
  while (i < line.size() && (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')) {
    ++i;
  }
  return line.substr(i);
}

uint64_t catch_size(const std::string& line, const char* prefix) {
  uint64_t prefix_length = strlen(prefix);
  std::string num = line.substr(prefix_length, line.size() - prefix_length - 2);
  return std::stoi(num);
}

Type* interpret_as_primitive_type(const std::string& line) {
  Type* type = nullptr;
  // yeah, I'm lazy as heck
  if (line.starts_with("unsigned ")) {
    type = interpret_as_primitive_type(remove_piece(line, "unsigned "));
    type->is_signed = false;
  } else if (line.starts_with("signed ")) {
    type = interpret_as_primitive_type(remove_piece(line, "signed "));
    type->is_signed = true;
  } else if (line.starts_with("long long")) {
    type = interpret_as_primitive_type(remove_piece(line, "long long"));
    type->size = 64;;
  } else if (line.starts_with("long ")) {
    type = interpret_as_primitive_type(remove_piece(line, "long "));
  } else if (line.starts_with("short")) {
    type = interpret_as_primitive_type(remove_piece(line, "short"));
    type->size = 16;
  } else if (line == "short_t") {
    type = interpret_as_primitive_type("short");
  } else if (line == "int_t") {
    type = interpret_as_primitive_type("int");
  } else if (line == "long_t") {
    type = interpret_as_primitive_type("long");
  } else if (line == "float_t") {
    type = interpret_as_primitive_type("float");
  } else if (line == "double_t") {
    type = interpret_as_primitive_type("double");
  } else if (line.starts_with("int") && line.ends_with("_t")) {
    type = Type::New(INTEGER_TYPE);
    type->size = catch_size(line, "int");
    type->is_signed = true;
  } else if (line.starts_with("uint") && line.ends_with("_t")) {
    type = Type::New(INTEGER_TYPE);
    type->size = catch_size(line, "uint");
    type->is_signed = false;
  }  else if (line == "char") {
    type = Type::New(INTEGER_TYPE);
    type->size = 8;
    type->is_signed = true;
  } else if (line == "size_t") {
    type = Type::New(INTEGER_TYPE);
    type->size = 64;
    type->is_signed = false;
  } else if (line == "int") {
    type = Type::New(INTEGER_TYPE);
    type->size = 32;
    type->is_signed = true;
  } else if (line == "float") {
    type = Type::New(DOUBLE_TYPE);
    type->size = 32;
  } else if (line == "double") {
    type = Type::New(DOUBLE_TYPE);
    type->size = 64;
  } else if (line == "void") {
    type = Type::New(VOID_TYPE);
  } else if (line == "") {
    // for types in which "int" is a superfluous suffix
    type = Type::New(INTEGER_TYPE);
    type->size = 1;
    type->is_signed = true;
  } else {
    crash_on_line(line, "unable to interpret as primitive type");
  }
  assert(type != nullptr);
  return type;
}

Type* interpret_as_type(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node);

Type* interpret_as_struct_specifier(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  /*
   *(struct_specifier
      body: (field_declaration_list
              (field_declaration type: (primitive_type) declarator: (field_identifier))
              (field_declaration type: (primitive_type) declarator: (field_identifier))))
   * */
  Type* struct_type = Type::New(STRUCT_TYPE);
  TSNode body = ts_node_child_by_field_name(node, "body");
  if (body.id != nullptr) {
    uint64_t length = ts_node_named_child_count(body);
    for (uint64_t i = 0; i < length; ++i) {
      TSNode field_node = ts_node_named_child(body, i);
      TSNode type_node = ts_node_child_by_field_name(field_node, "type");
      assert(type_node.id != nullptr);
      TSNode declarator_node = ts_node_child_by_field_name(field_node, "declarator");
      assert(declarator_node.id != nullptr);
      Type* type = interpret_as_type(language, source_code, scope, type_node);
      while (strcmp(ts_language_symbol_name(language, ts_node_symbol(declarator_node)), "pointer_declarator") == 0) {
        Type* ptr = Type::New(POINTER_TYPE);
        ptr->subtype = type;
        type = ptr;
        declarator_node = ts_node_child_by_field_name(declarator_node, "declarator");
      }
      std::string name = ts_node_source_code(declarator_node, source_code);
      struct_type->fields.push_back({name, type});
    }
  }
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  if (name.id != nullptr) {
    Declaration* type_decl = Declaration::New(TYPE_DECL);
    type_decl->type = struct_type;
    type_decl->name = ts_node_source_code(name, source_code);
    scope->children.push_back(type_decl);

    Type* reference = Type::New(SYMBOL_TYPE);
    reference->symbol = Symbol { .identifiers = {type_decl->name} };
    return reference;
  }
  return struct_type;
}

Type* interpret_as_enum_specifier(const TSLanguage* /*language*/, const char* source_code, Declaration* scope, TSNode& node) {
  Type* enum_type = interpret_as_primitive_type("int");
  TSNode name = ts_node_child_by_field_name(node, "name");
  if (name.id != nullptr) {
    Declaration* type_decl = Declaration::New(TYPE_DECL);
    type_decl->type = enum_type;
    type_decl->name = ts_node_source_code(name, source_code);
    scope->children.push_back(type_decl);

    Type* reference = Type::New(SYMBOL_TYPE);
    reference->symbol = Symbol { .identifiers = {type_decl->name} };
    return reference;
  }
  return enum_type;
}

Type* interpret_as_type(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  std::string symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
  Type* type = nullptr;
  
  if (symbol_name == "sized_type_specifier") {
    type = interpret_as_primitive_type(ts_node_source_code(node, source_code));
  } else if (symbol_name == "primitive_type") {
    type = interpret_as_primitive_type(ts_node_source_code(node, source_code));
  } else if (symbol_name == "struct_specifier") {
    type = interpret_as_struct_specifier(language, source_code, scope, node);
  } else if (symbol_name == "union_specifier") {
    type = interpret_as_struct_specifier(language, source_code, scope, node);
  } else if (symbol_name == "identifier") {
    type = Type::New(SYMBOL_TYPE);
    type->symbol = Symbol { .identifiers = {ts_node_source_code(node, source_code)} };
  } else if (symbol_name == "enum_specifier") {
    type = interpret_as_enum_specifier(language, source_code, scope, node);
  } else {
    crash_on_node(node, source_code, symbol_name, "unable to interpret as type");
  }
  assert(type != nullptr);
  return type;
}

void explore_as_type_definition(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  TSNode type_node = ts_node_child_by_field_name(node, "type");
  assert(type_node.id != nullptr);
  TSNode declarator_node = ts_node_child_by_field_name(node, "declarator");
  assert(declarator_node.id != nullptr);
  Type* type = interpret_as_type(language, source_code, scope, type_node);
  while (strcmp(ts_language_symbol_name(language, ts_node_symbol(declarator_node)), "pointer_declarator") == 0) {
    Type* ptr = Type::New(POINTER_TYPE);
    ptr->subtype = type;
    type = ptr;
    declarator_node = ts_node_child_by_field_name(declarator_node, "declarator");
  }
  std::string name = ts_node_source_code(declarator_node, source_code);
  Declaration* type_decl = Declaration::New(TYPE_DECL);
  type_decl->type = type;
  type_decl->name = name;
  scope->children.push_back(type_decl);
}

void explore_as_function_declaration(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  TSNode type_node = ts_node_child_by_field_name(node, "type");
  assert(type_node.id != nullptr);
  TSNode function_declarator_node = ts_node_child_by_field_name(node, "declarator");
  assert(function_declarator_node.id != nullptr);
  TSNode declarator_node = ts_node_child_by_field_name(function_declarator_node, "declarator");
  assert(declarator_node.id != nullptr);

  Type* type = interpret_as_type(language, source_code, scope, type_node);
  Declaration* func_decl = Declaration::New(FUNCTION_DECL);
  func_decl->type = type;

  while (strcmp(ts_language_symbol_name(language, ts_node_symbol(function_declarator_node)), "pointer_declarator") == 0) {
    Type* ptr = Type::New(POINTER_TYPE);
    ptr->subtype = func_decl->type;
    func_decl->type = ptr;
    function_declarator_node = ts_node_child_by_field_name(function_declarator_node, "declarator");
    declarator_node = ts_node_child_by_field_name(function_declarator_node, "declarator");
  }
  std::string name = ts_node_source_code(declarator_node, source_code);
  func_decl->name = name;

  TSNode parameters = ts_node_child_by_field_name(function_declarator_node, "parameters");
  if (parameters.id != nullptr) {
    uint64_t length = ts_node_named_child_count(parameters);
    for (uint64_t i = 0; i < length; ++i) {
      TSNode parameter_node = ts_node_named_child(parameters, i);
      TSNode type_node = ts_node_child_by_field_name(parameter_node, "type");
      assert(type_node.id != nullptr);
      TSNode declarator_node = ts_node_child_by_field_name(parameter_node, "declarator");
      Type* type = interpret_as_type(language, source_code, scope, type_node);
      std::string name = "_";
      if (declarator_node.id != nullptr) {
        if (strcmp(ts_language_symbol_name(language, ts_node_symbol(declarator_node)), "abstract_pointer_declarator") == 0) {
          Type* ptr = Type::New(POINTER_TYPE);
          ptr->subtype = type;
          type = ptr;
        } else {
          while (strcmp(ts_language_symbol_name(language, ts_node_symbol(declarator_node)), "pointer_declarator") == 0) {
            Type* ptr = Type::New(POINTER_TYPE);
            ptr->subtype = type;
            type = ptr;
            declarator_node = ts_node_child_by_field_name(declarator_node, "declarator");
          }
          name = ts_node_source_code(declarator_node, source_code);
        }
      }
      func_decl->parameters.push_back({name, type});
    }
    scope->children.push_back(func_decl);
  } else {
    std::string symbol_name = ts_language_symbol_name(language, ts_node_symbol(node));
    crash_on_node(node, source_code, symbol_name, "unable to explore as function declaration");
  }
}
 
void explore_as_scope(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  uint64_t n_of_children = ts_node_named_child_count(node);
  for (uint64_t i = 0; i < n_of_children; ++i) {
    TSNode child = ts_node_named_child(node, i);
    explore(language, source_code, scope, child);
  }
}

void explore_as_struct_specifier(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  Type* reference = interpret_as_struct_specifier(language, source_code, scope, node);
  Type::Delete(reference);
}

void explore_as_enum_specifier(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  Type* reference = interpret_as_enum_specifier(language, source_code, scope, node);
  Type::Delete(reference);
}

typedef void(*explorer)(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node);
std::unordered_map<std::string, explorer> explorers = {
  {"translation_unit", explore_as_scope},
  {"type_definition", explore_as_type_definition},
  {"declaration", explore_as_function_declaration},
  {"function_definition", explore_as_function_declaration},
  {"struct_specifier", explore_as_struct_specifier},
  {"enum_specifier", explore_as_enum_specifier},
  {"union_specifier", explore_as_struct_specifier}
};

void explore(const TSLanguage* language, const char* source_code, Declaration* scope, TSNode& node) {
  assert(node.id != nullptr);
  std::string symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
  auto it = explorers.find(symbol_name);
  if (it != explorers.end()) {
    it->second(language, source_code, scope, node);
  } else {
    crash_on_node(node, source_code, symbol_name, "unable to explore");
  }
}

bool parse_source_code(TSParser* parser, const TSLanguage* language, const char* source_code) {
  TSTree *tree = ts_parser_parse_string(parser, NULL, source_code, strlen(source_code));
  TSNode root_node = ts_tree_root_node(tree);
  Declaration* scope = Declaration::New(MODULE_DECL);
  explore(language, source_code, scope, root_node);
  Declaration::Print(std::cout, scope);
  ts_tree_delete(tree);
  return true;
}

API::Result API::cpp(const std::vector<std::string>& c_files) {
  TSParser* parser = ts_parser_new();
  const TSLanguage* language = tree_sitter_c();
  ts_parser_set_language(parser, language);

  for (std::string filepath : c_files) {
    char* source_code = read_source_code(filepath.c_str());
    assert(source_code != nullptr);
    parse_source_code(parser, language, source_code);
    free(source_code);
  }

  ts_parser_delete(parser);
  return Result::OK;
}
