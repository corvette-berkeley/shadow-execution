
#include <stdio.h>

double fadd(double a, double b) {
  return a + b;
}

int main() {
  float x = 1.3;
  float y = 1.123456789;

  x = x + 0.0; // to be able to inspect trace
  printf("x: %.10f\n", x);

  double result = fadd(x, y);
  printf("First result: %.10f\n", result);

  result = result + x;
  printf("Second result: %.10f\n", result);

  return 0;
}
