
#include <stdlib.h>
#include <stdio.h> 

int main() {
  int* a = (int*) malloc(sizeof(int)*10);

  int i = 0;
  for(; i < 10; i++) {
    a[i] = i;
  }

  short *b = (short*)a;
  short *c = b + 3;
  long *d = (long*)c;
  long result = *(d+1);

  printf("e: %ld", result);

  return 0;
}
