
#include <stdio.h>

int main() {
  float array[5];
  int i, j;
  float x = 1.123456789;
  float y = 1.3;

  for(i = 0; i < 5; i++) {
    array[i] = x + y;
  }

  printf("array[0]: %0.10f\n", array[0]);
  return 0;
}
