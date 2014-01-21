
#include <stdlib.h>
#include <stdio.h>

int main() {
  int *a = (int*)malloc(sizeof(int)*2);
  a[0] = 2;
  a[1] = 3;
  long *b = (long*)a;
  long result = *b;
  printf("result: %ld\n", result);

  a[0] = 3;
  a[1] = 2;
  long result2 = *b;
  printf("result2: %ld\n", result2);

  return 0;
}
