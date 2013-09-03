
#include <stdio.h>
#include <math.h>

int main() {
  float x = 4.0;
  x = x + 6.0;
  x = x - 3.0;
  float result = logf(x);
  printf("Result %f\n", result);
  return 0;
}
