
#include <stdio.h>

float fadd(float a, float b) {
  return a + b;
}

int main() {
  float x = 1.3d;
  float y = 1.3d;
  float result = fadd(x, y);
  printf("%.10f\n", result);
  return 0;
}
