#include <stdlib.h>
#include <stdio.h>

int main() {
  double* arr = (double*) malloc(5 * sizeof(double));
  int i;

  for (i = 0; i < 4; i++) {
    arr[i] = i;
  }

  float* farr = (float*) arr;
  farr = farr + 34513;
  farr = farr + 5;
  farr = farr - 34513;

  float f = *farr;

  return 0;
}
