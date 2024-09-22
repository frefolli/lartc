#include <stdio.h>
#include <stdint.h>

void printf_str(char* fmt, char* str) {
  printf(fmt, str);
}

void printf_i64(char* fmt, int64_t x) {
  printf(fmt, x);
}
