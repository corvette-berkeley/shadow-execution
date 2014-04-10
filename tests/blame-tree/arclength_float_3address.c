#include <math.h>
#include <stdio.h>

float fun(float x) {
  int k, n = 5;
  float t1, d1 = 1.0L;
  float temp1, temp2, temp3;
  t1 = x; // stmt 1
  
  for(k = 1; k <= n; k++) {
    d1 = 2.0 * d1; // stmt 2
    temp1 = d1 * x; // stmt 3
    temp2 = sin(temp1); //stmt 4
    temp3 = temp2 / d1; // stmt 5
    t1 = t1 + temp3; // stmt 6
  }
  return t1;
}


int main( int argc, char **argv) {
  int i, j, k, n = 5;
  float h, t1, t2, dppi, ans = 5.795776322412856L;
  //double s1, threshold = 1e-14L;
  float threshold = 1e-14L;
  float s1;
  float temp1, temp2, temp3, temp4, temp5, temp6;

  t1 = -1.0; // stmt 7
  dppi = acos(t1); // stmt 8
  s1 = 0.0; // stmt 9
  t1 = 0.0; // stmt 10
  h = dppi / n; // stmt 11

  for(i = 1; i <= n; i++) {
    temp1 = i * h; // stmt 12
    t2 = fun(temp1); // stmt 13
    temp2 = t2 - t1; // stmt 14
    temp3 = temp2 * temp2; // stmt 15
    temp4 = h * h; // stmt 16
    temp5 = temp4 + temp3; // stmt 17
    temp6 = sqrt(temp5); // stmt 18
    s1 = s1 + temp6; // stmt 19
    t1 = t2; // stmt 20
  }

  printf(" => total error: %d %1.15Le %1.15Le\n", (long double) fabs(ans-s1) > threshold,
	 (long double) fabs(ans-s1), (long double)threshold);
  printf(" => ans: %1.15Le\n =>  s1: %1.15Le\n", (long double)ans, (long double)s1);

  return 0;

}


