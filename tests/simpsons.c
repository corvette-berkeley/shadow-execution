#include <time.h>
#include <stdarg.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

double fun(double x) {
  double pi = acos(-1.0);
  double result;
  result = sin(pi * x);
  return result;
}

int main( int argc, char **argv) {
  int i, j, k; // diff: added constants
  double a, b; // removed fun, h, s1, and x
  double h, s1, x;
  const int n = 1000;
  const double fuzz = 1e-26; // diff: added fuzz

  a = 0.0;
  b = 1.0;
  h = (b - a) / (2.0 * n);
  s1= 0.0;

  x = a;
  s1 = fun(x);

L100:
  x = x + h;
  s1 = s1 + 4.0 * fun(x);
  x = x + h;
  if (x + fuzz >= b) goto L110;
  s1 = s1 + 2.0 * fun(x);
  goto L100;

L110:
  s1 = s1 + fun(x);

  return 0;
}
