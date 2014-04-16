#include <time.h>
#include <stdarg.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

float fun(float x) {
  float pi; 
  float result;
  float tmp1;
  
  pi = acos(-1.0f);
  tmp1 = pi * x;
  result = sin(tmp1);

  return result + 0;
}

int main( int argc, char **argv) {

  int i;
  int l;
  const int n = 10000;
  const float fuzz = 1e-26; 

  float a, b; 
  float h, s1, x;
  float tmp1, tmp2, tmp3, tmp4, tmp5;

  i = 0;

  for(l = 0; l < 100; l++) { // ITERS before
    a = 0.0f;
    b = 1.0f;
    tmp1 = b - a;
    tmp2 = 2.0f * n;
    h = tmp1 / tmp2;

    s1= 0.0f;

    x = a;

    s1 = fun(x);

  L100:
    i = i + 1;
    x = x + h;
    tmp3 = 4.0f * fun(x);
    s1 = s1 + tmp3;
    x = x + h;
    tmp4 = x + fuzz;

    if (tmp4 >= b) goto L110;

    tmp5 = 2.0 * fun(x);
    s1 = s1 + tmp5;
    goto L100;

  L110:
    s1 = s1 + fun(x);

  }

  printf("i = %d\n", i);
  printf("s1 = %.10f\n", s1);

  return 0;
}
