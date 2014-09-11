#include <stdio.h>

int main() {
  float a = 4.0f;
  float b = 4.1f;
  float c = 8.0f;
  float d = 2.0f; 
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;

  tmp0 = a + b;
  tmp1 = tmp0 + c;
  tmp2 = tmp1 / 2; // s

  tmp3 = tmp2 - a;
  tmp4 = tmp2 - b;
  tmp5 = tmp2 - c;

  tmp6 = tmp2*tmp3;
  tmp7 = tmp6*tmp4;
  tmp8 = tmp7*tmp5;

  printf("s = %.10f\n", tmp8);

  return tmp8;
}
