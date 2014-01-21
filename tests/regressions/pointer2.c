#include <stdlib.h>
#include <stdio.h>

int main()
{
  long long x = 2;
  int* p = (int *) &x;
  p = p + 1;
  int z = *p;

  printf("%d\n", z);

  return 0;
}
