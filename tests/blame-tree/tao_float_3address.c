#include <stdio.h>

int main() {
  float a, b;
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;

  a = 1.84089642f;

  // a*a*a*a
  tmp0 = a*a;               
  tmp1 = tmp0*a;            
  tmp2 = tmp1*a;         

  // 4*a*a*a
  tmp3 = 4 * a;          
  tmp4 = tmp3 * a;
  tmp5 = tmp4 * a;      

  // 6*a*a
  tmp6 = 6 * a;
  tmp7 = tmp6 * a;

  // 4 *a
  tmp8 = 4 *a;

  tmp9 = tmp2 - tmp5;
  tmp10 = tmp9 + tmp7;
  tmp11 = tmp10 - tmp8;

  tmp12 = tmp11 + 1;

  b = tmp12;

  if (b > 0.499999)
    printf("hit\n");
  else
    printf("miss\n");

  printf("result: %.10f\n", b);
  return 0;
}
