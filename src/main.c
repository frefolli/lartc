#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <tree_sitter/api.h>

// Declare the `tree_sitter_lart` function, which is
// implemented by the `tree-sitter-lart` library.
const TSLanguage *tree_sitter_lart(void);

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
    text = malloc(fsize + 1);
    fread(text, fsize, 1, file);
    fclose(file);
    return text;
}

int main(int argc, char** args) {
  // Create a parser.
  TSParser *parser = ts_parser_new();

  // Set the parser's language (JSON in this case).
  ts_parser_set_language(parser, tree_sitter_lart());

  // Build a syntax tree based on source code stored in a string.
  char *source_code = read_source_code(argc, args);
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );

  // Get the root node of the syntax tree.
  TSNode root_node = ts_tree_root_node(tree);

  // Get some child nodes.
  TSNode array_node = ts_node_named_child(root_node, 0);
  TSNode number_node = ts_node_named_child(array_node, 0);

  // Check that the nodes have the expected types.
  assert(strcmp(ts_node_type(root_node), "source_file") == 0);

  // Print the syntax tree as an S-expression.
  char *string = ts_node_string(root_node);
  printf("Syntax tree: %s\n", string);

  // Free all of the heap-allocated memory.
  free(string);
  ts_tree_delete(tree);
  ts_parser_delete(parser);
  free(source_code);
  return 0;
}
