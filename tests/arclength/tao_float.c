#include <stdio.h>

int main() {
  float a, b;

  a = 1.84089642;
  b = a*a*a*a - 4 *a*a*a + 6*a*a - 4*a + 1;
  if (b > 0.499999)
    printf("hit\n");
  else
    printf("miss\n");

  printf("result: %lf\n", b);
  return 0;
}
