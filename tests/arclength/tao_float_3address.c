#include <stdio.h>

int main() {
  float a, b;
  float tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;

  a = 1.84089642;

  // a*a*a*a
  tmp1 = a*a;               
  tmp2 = tmp1*a;            
  tmp3 = tmp2*a;         

  // 4*a*a*a
  tmp4 = 4 * a;          
  tmp5 = tmp4 * a;
  tmp6 = tmp5 * a;      

  // 6*a*a
  tmp7 = 6 * a;
  tmp8 = tmp7 * a;

  // 4 *a
  tmp9 = 4 *a;

  tmp10 = tmp3 - tmp6;
  tmp11 = tmp10 + tmp8;
  tmp12 = tmp11 - tmp9;

  b = tmp12 + 1;

  if (b > 0.499999)
    printf("hit\n");
  else
    printf("miss\n");

  printf("result: %lf\n", b);
  return 0;
}
