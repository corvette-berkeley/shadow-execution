
#include <stdlib.h>
#include <stdio.h> 

int main() {
  double* a = (double*) malloc(sizeof(double)*10);

  int i = 0;
  for(; i < 10; i++) {
    a[i] = i + 0.33;
  }
  double result = a[3];

  
  float* b = (float*)(a + 2);
  
  float c = *b;
  
  float* d = (float*)a;
  
  float e = *(d+2);
  float f = *(d+3);

  printf("c: %f, e: %f, f: %f", c, e, f);
  
  return 0;
}
