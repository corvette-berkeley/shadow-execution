#include <stdlib.h>
#include <stdio.h> 

struct X {
  int x;
  char c;
  long l;
};

int main() {
  struct X *x = (struct X *)malloc(sizeof(struct X)*10);
  x[7].c = 3;
  return 0;
} 
