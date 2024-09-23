# LartC - Compiler for Lart Programming Language

Static typed prototype programming language made in a two month journey.
Checkout [lart-examples](https://github.com/frefolli/lart-examples) repository for examples of usage of both the compiler and the language.

# Help

```
Usage: lartc [options] file...
Options:
  --help                   Display this information.
  --version                Display compiler version information.

  -v                       Display the programs invoked by the compiler.
  -E                       Preprocess only; do not compile, assemble or link.
  -S                       Compile only; do not assemble or link.
  -c                       Compile and assemble, but do not link.
  -o <file>                Place the output into <file>.

  -d                       Dumps debug information to stdout and to './tmp' directory.

For bug reporting, please see:
<https://github.com/frefolli/lartc/issues>.
```

# Example usage

`./builddir/lartc ../lart-examples/hello.lart -o main.exe`

Will produce an executable which output is:

```
00000000000000000000000000000001
00000000000000000000000000000010
00000000000000000000000000000110
00000000000000000000000000001010
00000000000000000000000000011110
00000000000000000000000000100010
00000000000000000000000001100110
00000000000000000000000010101010
00000000000000000000000111111110
00000000000000000000001000000010
00000000000000000000011000000110
00000000000000000000101000001010
00000000000000000001111000011110
00000000000000000010001000100010
00000000000000000110011001100110
00000000000000001010101010101010
00000000000000011111111111111110
00000000000000100000000000000010
00000000000001100000000000000110
00000000000010100000000000001010
00000000000111100000000000011110
```
