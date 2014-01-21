#include <stdio.h>

double a = 8;
double b;

int main() {

  b = a;
  b += 5.5;
  double y = b;
  double z = b * y;

  printf("Value of a is %f\n", z); 
  return 0;
}
