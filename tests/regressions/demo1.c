#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * This example covers:
 * 1. Call to malloc.
 * 2. Array accesses.
 * 3. Call to non-void interpreted function with pointer parameter.
 * 4. Call to uninterpreted function sqrt.
 * 5. Call to uninterpreted function printf.
 */

#define N 3

double array_sum(double *array) {
  int i;
  double sum = 0.0;

  for(i = 0; i < N; i++) {
    sum += array[i];
  }
  return sum;
}

int main() {
  double *array = malloc(sizeof(double)*N);
  double result, sum;
  int i;

  for(i = 0; i < 2*N; i++) {
    array[i] = i + 0.5;
  }

  sum = array_sum(array);
  result = sqrt(sum);
  
  /* More advanced operations (casting and pointer arithmetic) */
  float *farray = (float *)(array + 1);
  float fresult = farray[1];

  /* Even more advanced operations (uninterpreted functions require syncing) */
  float *anotherFArray = malloc(sizeof(float)*N);
  memcpy(anotherFArray, farray, sizeof(float)*N);
  float fAnotherResult = anotherFArray[1];

  /* Flatten representation of multiple dimensional array and struct */
  struct point {
    int x;
    double y[2];
  } p;

  p->y[1] = 5.0;

  return 0;
}
