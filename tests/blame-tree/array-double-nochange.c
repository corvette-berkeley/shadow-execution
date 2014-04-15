
#include <stdio.h>

int main() {
  double array[5];
  int i, j;
  float fx = 1.23456789;
  float fy = 1.3;
  double x = (double)fx;
  double y = (double)fy;

  for(i = 0; i < 5; i++) {
    array[i] = x + y;
  }

  printf("array[0]: %0.10f\n", array[0]);
  return 0;
}
