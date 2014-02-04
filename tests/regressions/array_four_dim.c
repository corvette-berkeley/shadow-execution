#include <stdio.h>

double x[5][5][5][5];

int main() {
  x[0][0][0][0] = 1.0;
  x[4][4][4][4] = 2.0;
  double y = x[4][4][4][4];
  return 0;
}
