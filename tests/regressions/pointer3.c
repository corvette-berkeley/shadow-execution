#include <stdlib.h>
#include <stdio.h>

int main()
{
  long long x = 2;
  long long* p = (long long *) &x;
  p = p + 1;
  long long z = *p;

  printf("%d\n", z);
}
