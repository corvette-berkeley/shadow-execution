
#include <stdlib.h>
#include <stdio.h> 


struct X {
  int a;
  double b;
} __attribute__((packed));

struct Y {
  int c;
} __attribute__((packed));


int main() {
  
  struct X* x = (struct X*)malloc(sizeof(struct X) + sizeof(struct Y));
  printf("size of X %d\n", sizeof(struct X));
  printf("size of Y %d\n", sizeof(struct Y));
  printf("address of x %llu\n", (unsigned long long) x);
  x->a = 3;
  x->b = 5.5;
  struct Y* y = (struct Y*)(x+1);
  printf("address of y %llu\n", (unsigned long long) y);
  y->c = 6;

  return 0;
}
