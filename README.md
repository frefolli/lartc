# LartC - Compiler for Lart Programming Language

## TODO

### Expression Type Checking and Reference Resolution

In binary expressions with dot_operator the left operand should be searched first, then extracted it's type, search inside it the right symbol;
In other expressions compute the type inside a cache map and check type availabilities using both type resolution and reference resolution obtain via a prior global object scan.
