#include <stdio.h>

double x[5][5];

int main() {
  x[0][0] = 1.0;
  x[1][2] = 2.0;
  double y = x[1][2];
  return 0;
}
