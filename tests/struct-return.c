
#include <stdlib.h>
#include <stdio.h> 


struct X {
  int a;
  double b;
};


struct X foo() {
  struct X x = {1, 2.5};
  return x;
}


int main() {
  
  struct X y = foo();
  printf("%d\n", y.a);

  return 0;
}
