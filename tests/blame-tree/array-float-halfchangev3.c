
#include <stdio.h>

int main() {
  float array[6];
  int i, j;

  float x = 1.3;
  float y = 1.123456789;
  float z = 1.23456789;

  for(i = 0; i < 6; i++) {
    if (i % 2 == 0) {
      // even elements of array require double precision
      array[i] = x + y;
    }
    else {
      // odd elements do not require double precision
      array[i] = x + z;
    }
  }
  
  for(i = 0; i < 6; i++) {
    printf("array[%d]: %0.10f\n", i, array[i]);
  }
  return 0;
}
