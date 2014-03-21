#include <math.h>
#include <stdio.h>

double fun(double x){
  int k, n = 5;
  double t1, d1 = 1.0L;
  double temp1, temp2, temp3;
  t1 = x;
  
  for(k = 1; k <= n; k++) {
    d1 = 2.0 * d1;
    temp1 = d1 * x;
    temp2 = sin(temp1);
    temp3 = temp2 / d1;
    t1 = t1 + temp3;
  }
  return t1;
}


int main( int argc, char **argv) {
  int i, j, k, n = 5;
  double h, t1, t2, dppi, ans = 5.795776322412856L;
  //double s1, threshold = 1e-14L;
  double threshold = 1e-14L;
  long double s1;
  double temp1, temp2, temp3, temp4, temp5, temp6;

  t1 = -1.0;
  dppi = acos(t1);
  s1 = 0.0;
  t1 = 0.0;
  h = dppi / n;

  for(i = 1; i <= n; i++) {
    temp1 = i * h;
    t2 = fun(temp1);
    temp2 = t2 - t1;
    temp3 = temp2 * temp2;
    temp4 = h * h;
    temp5 = temp4 + temp3;
    temp6 = sqrt(temp5);
    s1 = s1 + temp6;
    t1 = t2;
  }

  printf(" => total error: %d %1.15Le %1.15Le\n", (long double) fabs(ans-s1) > threshold,
	 (long double) fabs(ans-s1), (long double)threshold);
  printf(" => ans: %1.15Le\n =>  s1: %1.15Le\n", (long double)ans, (long double)s1);

  return 0;

}


