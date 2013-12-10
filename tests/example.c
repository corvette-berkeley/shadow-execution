
#include <stdio.h>
int* foo(int *p);

int main() {
  int x = 12, y;
  int *q = &x;
  int *r = foo(q);
  if (q == r) {
    y = x + 1;
  }
  else {
    y = x - 1;
  }
  printf("%p\n", r);
  printf("%p\n", q);
  printf("%d\n",*q);
  printf("%d\n",*r);

  return 0;
}
