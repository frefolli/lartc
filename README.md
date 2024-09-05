# LartC - Compiler for Lart Programming Language

## TODO

### Use type_ckec_ok to stop lazily before having a segfault

### Function Declaration / Definition

When adding a Function to children of module (in general a scoping declaration), check if such Function has been declared / defined before and:

- if declared after declaration:
  - check if signatures match and skip the insertion of the last declaration
- if defined after declaration:
  - check if signatures match and replace the declaration with the definition
- if declared after definition:
  - check if signatures match and skip the insertion of the declaration
- if defined after definition:
  - throw redefinition error

### Expression Type Checking and Reference Resolution

In binary expressions with dot_operator the left operand should be searched first, then extracted it's type, search inside it the right symbol;
In other expressions compute the type inside a cache map and check type availabilities using both type resolution and reference resolution obtain via a prior global object scan.
