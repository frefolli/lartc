#include <assert.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <iostream>
#include <string>
#include <vector>

extern "C" const TSLanguage *tree_sitter_lart(void);

char* read_source_code(int argc, char** args) {
    char* text = NULL;
    const char* filepath = "hello.rs";
    if (argc > 1) {
        filepath = args[1];
    }
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = (char*) malloc(fsize + 1);
    fread(text, fsize, 1, file);
    fclose(file);
    return text;
}


struct Type {
  struct Field {
    std::string name;
    Type* type;
  };
  std::vector<Field*> fields;
  uint64_t size;
  bool is_signed;
  Type* subtype;
  enum {
    VOID, INTEGER, DOUBLE, STRUCT, POINTER
  } kind;
};

Type* new_void_type() {
  return new Type {
    .fields = {},
    .size = 0,
    .is_signed = false,
    .subtype = nullptr,
    .kind = Type::VOID
  };
}

Type* new_integer_type(uint64_t size, bool is_signed) {
  return new Type {
    .fields = {},
    .size = size,
    .is_signed = is_signed,
    .subtype = nullptr,
    .kind = Type::INTEGER
  };
}

Type* new_double_type(uint64_t size) {
  return new Type {
    .fields = {},
    .size = size,
    .is_signed = false,
    .subtype = nullptr,
    .kind = Type::DOUBLE
  };
}

Type* new_struct_type(std::vector<Type::Field*> fields) {
  return new Type {
    .fields = fields,
    .size = 0,
    .is_signed = false,
    .subtype = nullptr,
    .kind = Type::STRUCT
  };
}

Type* new_pointer_type(Type* subtype) {
  return new Type {
    .fields = {},
    .size = 0,
    .is_signed = false,
    .subtype = subtype,
    .kind = Type::POINTER
  };
}

void print_type(Type* type);
void print_field(Type::Field* field) {
  std::cout << "(field " << field->name << " ";
  print_type(field->type);
  std::cout << ")";
}

void delete_type(Type* type);
void delete_field(Type::Field* field) {
  if (field != nullptr) {
    delete_type(field->type);
    delete field;
  }
}

void print_type(Type* type) {
  if (type != nullptr) {
    switch (type->kind) {
      case Type::VOID: {
        std::cout << "(void)" << std::endl;
      }; break;
      case Type::INTEGER: {
        std::cout << "(integer " << type->size << " " << type->is_signed << ")" << std::endl;
      }; break;
      case Type::DOUBLE: {
        std::cout << "(double " << type->size << ")" << std::endl;
      }; break;
      case Type::STRUCT: {
        std::cout << "(struct";
        for (Type::Field* field : type->fields) {
          std::cout << " ";
          print_field(field);
        }
        std::cout << ")" << std::endl;
      }; break;
      case Type::POINTER: {
        std::cout << "(pointer)" << std::endl;
      }; break;
    }
  }
}

void delete_type(Type* type) {
  if (type != nullptr) {
    switch (type->kind) {
      case Type::VOID: {
      }; break;
      case Type::INTEGER: {
      }; break;
      case Type::DOUBLE: {
      }; break;
      case Type::STRUCT: {
        for (Type::Field* field : type->fields) {
          delete_field(field);
        }
      }; break;
      case Type::POINTER: {
        delete_type(type->subtype);
      }; break;
    }
    delete type;
  }
}

struct Declaration {
  struct Parameter {
    std::string name;
    Type* type;
  };
  std::string name;
  std::vector<Declaration*> children;
  std::vector<Parameter*> parameters;
  Type* type;
  Declaration* body;
  enum {
    MODULE, TYPEDEF, FUNCTION
  } kind;
};

Declaration* new_module_declaration(std::string name,
                        std::vector<Declaration*> children) {
  return new Declaration {
    .name = name,
    .children = children,
    .parameters = {},
    .type = {},
    .body = nullptr,
    .kind = Declaration::MODULE
  };
}

Declaration* new_function_declaration(std::string name,
                          std::vector<Declaration::Parameter*> parameters,
                          Type* type,
                          Declaration* body) {
  return new Declaration {
    .name = name,
    .children = {},
    .parameters = parameters,
    .type = type,
    .body = body,
    .kind = Declaration::FUNCTION
  };
}

Declaration* new_typedef_declaration(std::string name,
                                     Type* type) {
  return new Declaration {
    .name = name,
    .children = {},
    .parameters = {},
    .type = type,
    .body = nullptr,
    .kind = Declaration::TYPEDEF
  };
}

void print_parameter(Declaration::Parameter* parameter) {
  if (parameter != nullptr) {
    std::cout << "(parameter " << parameter->name << " ";
    print_type(parameter->type);
    std::cout << ")";
  }
}

void delete_parameter(Declaration::Parameter* parameter) {
  if (parameter != nullptr) {
    delete_type(parameter->type);
    delete parameter;
  }
}

void print_declaration(Declaration* declaration) {
  if (declaration != nullptr) {
    switch (declaration->kind) {
      case Declaration::MODULE: {
        std::cout << "(module " << declaration->name;
        for (Declaration* child : declaration->children) {
          print_declaration(child);
        }
        std::cout<< ")" << std::endl;
      }; break;
      case Declaration::FUNCTION: {
        std::cout << "(function " << declaration->name << " (" << "))" << std::endl;
      }; break;
      case Declaration::TYPEDEF: {
        std::cout << "(typedef)" << std::endl;
      }; break;
    }
  }
}

void delete_declaration(Declaration* declaration) {
  if (declaration != nullptr) {
    switch (declaration->kind) {
      case Declaration::MODULE: {
        for (Declaration* child : declaration->children) {
          delete_declaration(child);
        }
      }; break;
      case Declaration::FUNCTION: {
        delete_declaration(declaration->body);
        for (Declaration::Parameter* parameter : declaration->parameters) {
          delete_parameter(parameter);
        }
        delete_type(declaration->type);
        delete_declaration(declaration->body);
      }; break;
      case Declaration::TYPEDEF: {
        delete_type(declaration->type);
      }; break;
    }
    delete declaration;
  }
}

void* visit_node(const TSLanguage* language, const char* text, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(language, ts_node_grammar_symbol(node));
  std::cout << symbol_name << std::endl;
  if (std::strcmp(symbol_name, "source_file") == 0) {
    std::vector<Declaration *> children = {};
    uint64_t child_count = ts_node_child_count(node);
    for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
      TSNode child = ts_node_named_child(node, child_index);
      children.push_back((Declaration*) visit_node(language, text, child));
    }
    return new_module_declaration("", children);
  } else if (std::strcmp(symbol_name, "module") == 0) {
    TSNode module_name = ts_node_child_by_field_name(node, "name", 4);
    uint64_t index = ts_node_start_byte(module_name);
    uint64_t len = ts_node_end_byte(module_name) - index;

    std::vector<Declaration *> children = {};
    uint64_t child_count = ts_node_child_count(node);
    for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
      TSNode child = ts_node_named_child(node, child_index);
      children.push_back((Declaration*) visit_node(language, text, child));
    }
    return new_module_declaration(std::string(text + index, len), children);
  }
  return nullptr;
}

int main(int argc, char** args) {
  TSParser* parser = ts_parser_new();
  const TSLanguage* lart =  tree_sitter_lart();
  ts_parser_set_language(parser, lart);
  char *source_code = read_source_code(argc, args);
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );
  TSNode root_node = ts_tree_root_node(tree);
  Declaration* global = (Declaration*) visit_node(lart, source_code, root_node);
  print_declaration(global);
  delete_declaration(global);

  ts_tree_delete(tree);
  ts_parser_delete(parser);
  free(source_code);
  return 0;
}
