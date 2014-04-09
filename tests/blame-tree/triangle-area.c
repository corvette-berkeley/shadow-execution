#include <stdio.h>

int main() {
  const float a = 4;
  const float b = 4.9; 
  const float c = 8.5;
  float s, r;


  s = (a+b+c)/2;
  r = s*(s-a)*(s-b)*(s-c);

  printf("r = %.10f\n", r);

  return 0;
}
