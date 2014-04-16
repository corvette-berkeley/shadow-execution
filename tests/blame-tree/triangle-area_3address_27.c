#include <stdio.h>

int main() {
  float a = 4;
  float b = 4.1; 
  float c = 8;
  float tmp4, tmp5;
  double tmp0, tmp1, tmp2; 
  double tmp3, tmp6, tmp7, tmp8;

  tmp0 = a + b;    // double
  tmp1 = tmp0 + c; // double
  tmp2 = tmp1 / 2; // s, double

  tmp3 = tmp2 - a; // double
  tmp4 = tmp2 - b; 
  tmp5 = tmp2 - c; 

  tmp6 = tmp2* tmp3; // double
  tmp7 = tmp6*tmp4; // double
  tmp8 = tmp7*tmp5; // double

  printf("s = %.10f\n", tmp8);

  return tmp8;
}
