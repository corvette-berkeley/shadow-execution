
#include <stdio.h>

float fadd(float a, float b) {
  return a + b;
}

int main() {
  float x = 1.3;
  float y = 1.3;
  float result = fadd(x, y);
  result = result + x;
  printf("%.10f\n", result);
  return 0;
}
