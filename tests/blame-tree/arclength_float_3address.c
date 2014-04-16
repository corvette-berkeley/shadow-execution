#include <math.h>
#include <stdio.h>

float fun(float x) {
  int k, n = 5;
  float t1, d1 = 1.0f;
  float temp1, temp2, temp3;
  t1 = x; // stmt 1
  
  for(k = 1; k <= n; k++) {
    d1 = 2.0 * d1; // stmt 2 
    temp1 = d1 * x; // stmt 3 // double
    temp2 = sin(temp1); //stmt 4 
    temp3 = temp2 / d1; // stmt 5 
    t1 = t1 + temp3; // stmt 6 // double
  }
  return t1 + 0; // double
}


int main( int argc, char **argv) {
  int i, j, k, n = 1000000;
  n = atoi(argv[1]);
  float h, t1, t2, dppi;
  float s1;
  float temp1, temp2, temp3, temp4, temp5, temp6;

  t1 = -1.0f; // stmt 7
  dppi = acos(t1); // stmt 8  // double
  s1 = 0.0f; // stmt 9
  t1 = 0.0f; // stmt 10
  h = dppi / n; // stmt 11 // double

  for(i = 1; i <= n; i++) {
    temp1 = i * h; // stmt 12 // double
    t2 = fun(temp1) + 0; // stmt 13 // double
    temp2 = t2 - t1; // stmt 14 // double
    temp3 = temp2 * temp2; // stmt 15 
    temp4 = h * h; // stmt 16// 
    temp5 = temp4 + temp3; // stmt 17 
    temp6 = sqrt(temp5); // stmt 18 
    s1 = s1 + temp6; // stmt 19 // double
    t1 = t2 + 0; // stmt 20 // double
  }

  printf("s1 = %.10f\n", s1);

//  printf(" => total error: %d %1.15Le %1.15Le\n", (long double) fabs(ans-s1) > threshold,
//	 (long double) fabs(ans-s1), (long double)threshold);
//  printf(" => ans: %1.15Le\n =>  s1: %1.15Le\n", (long double)ans, (long double)s1);

  return 0;

}


