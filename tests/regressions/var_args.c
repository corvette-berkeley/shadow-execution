#include <stdarg.h>
#include <stdio.h>


void foo2(int* resultbuf, size_t lengthp, char* format, va_list args) {
  int *x = resultbuf;
  return;
}


int main() {
  int y = 5;
  size_t size = 6;
  char *hello = "hello";
  va_list args;
  foo2(NULL, size, hello, args);
  return 0;

}

