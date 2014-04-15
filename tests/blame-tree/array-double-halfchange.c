
#include <stdio.h>

int main() {
  double array[6];
  int i, j;

  float fx = 1.3;
  float fy = 1.123456789;
  float fz = 1.23456789;
  double x = (double)fx;
  double y = (double)fy;
  double z = (double)fz;

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
