#include <math.h>
#include <stdio.h>

double fun(double x) {
  int k, n = 5;
  double d1 = 1.0L;
  long double t1;

  t1 = x;

  for(k = 1; k <= n; k++) {
    d1 = 2.0 * d1;
    t1 = t1 + sin (d1 * x) / d1;
  }

  return t1;
}

int main(int argc, char **argv) {

  int i, j, k, n = 1000000;
  double h, t1, t2, dppi, ans = 5.795776322412856L;
  double threshold = 1e-14L;
  long double s1;

  t1 = -1.0;
  dppi = acos(t1);
  s1 = 0.0;
  t1 = 0.0;
  h = dppi / n;

  for(i = 1; i <= n; i++) {
    t2 = fun (i * h);
    s1 = s1 + sqrt (h*h + (t2 - t1)*(t2 - t1));
    t1 = t2;
  }

  printf(" => total error: %d %1.15Le %1.15Le\n", (long double) fabs(ans-s1) > threshold, (long double) fabs(ans-s1), 
	 (long double)threshold);
  printf(" => ans: %1.15Le\n =>  s1: %1.15Le\n", (long double)ans, (long double)s1);

  return 0;
}


