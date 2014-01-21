#include <stdlib.h>
#include <stdio.h>

char* alloca_test() {
  char* var = alloca(10);
  *var = 7;
  *(var+1) = 7;

  return var;
}

long long foo() {
  char* c = malloc(10); ; 
  long long x = 0;
  long long y = 0;

  return x + y;
}

int main() {
  char* v = alloca_test();
  long long a = foo();

  printf("%d\n", (*v));
  printf("%d\n", *(v+1));
  printf("%d\n", (int)sizeof(char*));

  return (int) a;
}

