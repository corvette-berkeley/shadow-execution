#include <stdio.h>
#include <stdlib.h>

struct X {
  int a;
  double b;
};

struct X x;

int main() {

  int a = x.a;
  double b = x.b;

  printf("%d %f\n", a, b);
  return 0;
}
