
#include <stdlib.h>
#include <stdio.h> 


void foo(long b[5], int value) {
  int result = b[2];
  b[2] = value;
  return;
}


int bar(int x) {
  return x + 5;
}


int main() {

  int a[5];
  int i;
  for(i = 0; i < 5; i++) {
    a[i] = i;
  }

  int result = a[3];
  a[4] = 5;

  int x = bar(10);
  int value = 45;
  foo(a, value);

  printf("Result: %d", a[2]);
  return 0;
}
