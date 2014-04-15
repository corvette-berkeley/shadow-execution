
#include <stdio.h>

double fadd(double a, double b) {
  return a + b;
}

int main() {
  double x = 1.3;
  double y = 1.3;
  double result = fadd(x, y);
  result = result + x;
  printf("%.10f\n", result);
  return 0;
}
