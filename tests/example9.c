#include <stdio.h>
#include <stdlib.h>

double a = 8;
double b;
double c[5];
//double* d;

int main() {

  int i;
  for(i = 0; i < 5; i++) {
    c[i] = i;
  }

  b = a;
  b += 5.5;
  double y = b;
  double z = b * y;

  printf("Value of a is %f\n", z); 
  return 0;
}
