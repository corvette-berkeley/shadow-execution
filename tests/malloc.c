#include <stdlib.h>
#include <stdio.h>

int main() {
  int* a = (int*) malloc(sizeof(int)*3);
  
  for(unsigned i = 0; i < 3; i++) {
    a[i] = i+1;
  }

  int b = *(a+2);
  b = 5;

  printf("value of b is %d\n", *(a+2));

  return *(a+2);
}
