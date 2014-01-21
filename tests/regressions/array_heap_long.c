
#include <stdlib.h>
#include <stdio.h> 

int main() {
  long double* array = (long double*) malloc(5*sizeof(long double));

  int i = 0;
  for(; i < 5; i++) {
    array[i] = i + 0.3;
  }

  long double result = array[3];

  return 0;
}
