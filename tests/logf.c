
#include <stdio.h>
#include <math.h>

int main() {
  int y = 3;
  y = y * 2;
  y = y % 3;
  y = 4 << y;
  y = y << 4;
  float x = 4.0;
  x = x + 6.0;
  x = x - 3.0;
  x = x * 2.0;
  x = x / 1.2;
  float result = logf(x);
  if (x < 3.0)
    printf("Result %f\n", result);
  return 0;
}
