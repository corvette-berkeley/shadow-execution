#include <stdlib.h>
#include <stdio.h> 

struct X {
  int a;
  int b;
  int c;
};

int main() {
  struct X *x = (struct X *)malloc(sizeof(struct X)*10);
  printf("%ld\n", sizeof(struct X));
  x[7].c = 3;
  return 0;
} 
