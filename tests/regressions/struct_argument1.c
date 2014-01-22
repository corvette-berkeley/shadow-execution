
#include <stdlib.h>
#include <stdio.h> 


struct X {
  int a;
  double b;
};


void foo(struct X x, int a, double b) {
  int a1 = a;
  int b1 = b;
  x.a = a1;
  x.b = b1;
}


int main() {
  struct X y;
  foo(y, 1, 2.5);

  printf("%d\n", y.a);

  return 0;
}
