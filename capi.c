#include <stdio.h>
#include <stdint.h>

void printf_str(char* fmt, char* str) {
  printf(fmt, str);
}

void printf_i64(char* fmt, int64_t x) {
  printf(fmt, x);
}

void print_str(char* str) {
  printf("%s", str);
}

void print_char(char c) {
  putchar(c);
}

void print_num(int64_t num) {
  printf("%ld", num);
}
