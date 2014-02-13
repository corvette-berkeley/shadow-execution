#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * This example covers:
 * 1. Call to malloc.
 * 2. Array accesses.
 * 3. Call to non-void interpreted function with pointer parameter.
 * 4. Call to uninterpreted function sqrt.
 * 5. Call to uninterpreted function printf.
 */

double getelement(double *array, int i) {
  return array[i];
}

int main() {
  double array[3] = {1.5, 2.5, 3.5};
  double result; 

  result = getelement(array, 1);
  result = sqrt(result);
  
  /*
  // More advanced operations (casting and pointer arithmetic)
  float *farray = (float *)(array + 1);
  float fresult = farray[1];

  // Even more advanced operations (uninterpreted functions require syncing)
  float *anotherFArray = malloc(sizeof(float)*3);
  memcpy(anotherFArray, farray, sizeof(float)*3);
  float fAnotherResult = anotherFArray[1];

  // Flatten representation of multiple dimensional array and struct
  struct point {
    int x;
    double y[2];
  } p;

  p.y[1] = 5.0;
  */
  return 0;
}
