#include <stdio.h>

double x;

int main() {
  x = 5.5;
  double y = x;
  double z = x * y;

  printf("Value of a is %f\n", y); 
  return 0;
}
