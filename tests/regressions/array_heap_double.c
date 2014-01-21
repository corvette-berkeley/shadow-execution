
#include <stdlib.h>
#include <stdio.h> 

int main() {
  double* array = (double*) malloc(5*sizeof(double));

  int i = 0;
  for(; i < 5; i++) {
    array[i] = i + 0.3;
  }

  double result = array[3];

  return 0;
}
