#!/bin/bash

function run() {
  ./builddir/lartc ts.lart -l tree-sitter -l tree-sitter-lart -o main.exe && ./main.exe
}

function emit() {
  ./builddir/lartc ts.lart -E -o main.ll
}

function restore() {
  ./builddir/lartc main.ll -l tree-sitter -l tree-sitter-lart -o main.exe && ./main.exe
}

function inspect() {
  clang -emit-llvm -S main.c -o main.ll
}

function inquire() {
  clang++ -emit-llvm -S main.cc -o main.ll
}
