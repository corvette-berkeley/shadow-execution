
#include <stdio.h>

int* global;

int main() {

  int x = 5;
  global = &x;
  int* y = global;

  return 0;
}
