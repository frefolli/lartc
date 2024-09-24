#!/bin/bash

#SUBJECT="../lart-examples/ts.lart -ltree-sitter -ltree-sitter-lart"
#ARGS=../lart-examples/raylib.lart

#SUBJECT="../lart-examples/raylib.lart -lraylib"
#ARGS=

#ARGS=
#SUBJECT=../lart-examples/array_access.lart

#ARGS=
#SUBJECT=../lart-examples/hello.lart

#ARGS=
#SUBJECT=../lart-examples/function-pointer.lart

#ARGS=
#SUBJECT=types.lart

function run() {
  ./builddir/lartc $SUBJECT -o main.exe && ./main.exe $ARGS
}

function emit() {
  ./builddir/lartc $SUBJECT -E -o main.ll -d
}

function restore() {
  llc main.ll && as main.s && clang a.out -o main.exe && ./main.exe $ARGS
}

function inspect() {
  clang -emit-llvm -S main.c -o main.ll
}

function inquire() {
  clang++ -emit-llvm -S main.cc -o main.ll
}
