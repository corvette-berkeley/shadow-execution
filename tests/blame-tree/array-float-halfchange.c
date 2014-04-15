
#include <stdio.h>

int main() {
  float array[6];
  int i, j;

  float x = 1.3;
  float y = 1.123456789;
  float z = 1.23456789;

  // different result depending on precision
  for(i = 0; i < 3; i++) {
    array[i] = x + y;
  }

  // same result regardless precision
  for(i = 3; i < 6; i++) {
    array[i] = x + z;
  }
  
  for(i = 0; i < 6; i++) {
    printf("array[%d]: %0.10f\n", i, array[i]);
  }
  return 0;
}
