
#include <stdio.h>

double fadd(double a, double b) {
  return a + b;
}

int main() {
  float fx = 1.3;
  float fy = 1.123456789;
  double x = fx;
  double y = fy;

  x = x + 0.0;
  printf("x: %.10f\n", x);

  double result = fadd(x, y);
  printf("First result: %.10f\n", result);

  result = result + x;
  printf("Second result: %.10f\n", result);

  return 0;
}
