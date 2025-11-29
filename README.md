# LartC - Compiler for Lart Programming Language

Static typed prototype programming language made in a two month journey.
Checkout [lart-tests](https://github.com/frefolli/lart-tests) repository for examples of usage of both the compiler and the language.

# Help

```
Usage: lartc [options] file...
Options:
  -h/--help                Display this information.
  -V/--version             Display compiler version information.

  -v                       Display the programs invoked by the compiler.
  -E                       Preprocess only; do not compile, assemble or link.
  -S                       Compile only; do not assemble or link.
  -c                       Compile and assemble, but do not link.
  -o/--output <file>       Place the output into <file>.

  -d                       Dumps debug information to stdout and to './tmp' directory.

  -I<path>                 Add path to include directories.
  -Wg,<options>            Pass comma-separated <options> on to the generator.
  -Wa,<options>            Pass comma-separated <options> on to the assembler.
  -Wl,<options>            Pass comma-separated <options> on to the linker.
  -Xgenerator <arg>        Pass <arg> on to the generator.
  -Xassembler <arg>        Pass <arg> on to the assembler.
  -Xlinker <arg>           Pass <arg> on to the linker.

  Passing *.lart, *.ll, *.s, *.o, *.a, *.so files will cause them to be included in the correct phase of the compilation.
  As an experimental feature, passing *.h files will cause the compiler to parse those C-files and try to convert them to LART code (intended for headers). The result is printed to stdout, then the execution terminates.

For bug reporting, please see:
<https://github.com/frefolli/lartc/issues>
```

# Example usage

`./builddir/lartc -I../lart-tests/include ../lart-tests/tests/diff/hello/source.lart -o main.exe`

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

# Building

You'll need `tree-sitter-lart` (which can be found in my repos), `meson` and `llvm`. Optionally you could also compile it with whatever compiler you want, however with Clang for example a warning would arise (`-W-tautological-constant-out-of-range-compare`) since I'm checking in some parts of the code that the user of a function hasn't coerced a random int to enum (clang dislikes that) and so you would need to disable such warning (`CXXFLAGS=-Wno-tautological-constant-out-of-range-compare`).

I configured Meson to stop compile process if a warning from the C++ compile is arose (except ld warnings). Enjoy.

# Installing

Either with `packer` (my personal package building system) or with `make install`.
