
#include <stdio.h>
#include <math.h>

int main() {
  int y = 3;
  y = y * 2;
  float x = 4.0;
  x = x + 6.0;
  x = x - 3.0;
  x = x * 2.0;
  x = x / 1.2;
  float result = logf(x);
  printf("Result %f\n", result);
  return 0;
}
