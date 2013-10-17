
#include <stdlib.h>
#include <stdio.h> 


struct X {
  int a[2];
  double b;
};


struct X foo(long a, double b) {
  struct X x; // = {1, 2.5};
  x.a[0] = a;
  x.a[0] = a*2;
  x.b = b;
  return x;
}


int main() {
  
  struct X y = foo(100, 112);
  printf("%d\n", y.a[0]);

  return 0;
}
