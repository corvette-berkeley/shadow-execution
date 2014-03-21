#include <math.h>
#include <stdio.h>

double fun(double x){

  int k, n = 5;
  double t1, d1 = 1.0L;
  double temp1, temp2, temp3;

  long double l_t1, l_d1 = 1.0L;
  long double l_temp1, l_temp2, l_temp3;
  long double l_x = (long double)x;

  t1 = x;
  l_t1 = l_x; // shadow

  for(k = 1; k <= n; k++) {

    /** A STATEMENT **/
    printf("fun line: %d, arg d1: %Le\n", 1, (long double)fabs(l_d1 - d1));
    d1 = 2.0 * d1;
    l_d1 = 2.0 * l_d1; // shadow
    printf("fun line: %d, result dppi: %Le\n", 1, (long double)fabs(l_d1 - d1));

    /** A STATEMENT **/
    printf("fun line: %d, arg1 d1: %Le\n", 2, (long double)fabs(l_d1 - d1));
    printf("fun line: %d, arg2 x: %Le\n", 2, (long double)fabs(l_x - x));
    temp1 = d1 * x;
    l_temp1 = l_d1 * l_x; // shadow
    printf("fun line: %d, result temp1: %Le\n", 2, (long double)fabs(l_temp1 - temp1));

    /** A STATEMENT **/
    printf("fun line: %d, arg temp1: %Le\n", 3, (long double)fabs(l_temp1 - temp1));
    temp2 = sin(temp1);
    l_temp2 = sin(l_temp1); // shadow
    printf("fun line: %d, result temp2: %Le\n", 3, (long double)fabs(l_temp2 - temp2));

    /** A STATEMENT **/
    printf("fun line: %d, arg1 temp2: %Le\n", 4, (long double)fabs(l_temp2 - temp2));
    printf("fun line: %d, arg2 d1: %Le\n", 4, (long double)fabs(l_d1 - d1));
    temp3 = temp2 / d1;
    l_temp3 = l_temp2 / l_d1; // shadow
    printf("fun line: %d, result temp3: %Le\n", 4, (long double)fabs(l_temp3 - temp3));

    /** A STATEMENT **/
    printf("fun line: %d, arg1 t1: %Le\n", 5, (long double)fabs(l_t1 - t1));
    printf("fun line: %d, arg2 temp3: %Le\n", 5, (long double)fabs(l_temp3 - temp3));
    t1 = t1 + temp3;
    l_t1 = l_t1 + l_temp3; // shadow
    printf("fun line: %d, result t1: %Le\n", 5, (long double)fabs(l_t1 - t1));
  }

  return t1;
}


int main( int argc, char **argv) {

  int i, j, k, n = 10;
  double h, t1, t2, dppi, ans = 5.795776322412856L;
  //double s1, threshold = 1e-14L;
  double threshold = 1e-14L;
  long double s1;
  double temp1, temp2, temp3, temp4, temp5, temp6;

  long double l_h, l_t1, l_t2, l_dppi, l_ans = 5.795776322412856L;
  long double l_s1, l_threshold = 1e-14L;
  long double l_temp1, l_temp2, l_temp3, l_temp4, l_temp5, l_temp6;
  long double err = 1e-12L;

  t1 = -1.0;
  l_t1 = -1.0; // shadow

  dppi = acos(t1);
  l_dppi = acos(l_t1); // shadow

  s1 = 0.0;
  l_s1 = 0.0; // shadow

  t1 = 0.0;
  l_t1 = 0.0; // shadow

  /** A STATEMENT **/
  printf("line: %d, arg dppi: %Le\n", 0, (long double)fabs(l_dppi - dppi));
  h = dppi / n;
  l_h = l_dppi / n; // shadow
  printf("line: %d, result h: %Le\n", 1, (long double)fabs(l_h - h));

  for(i = 1; i <= n; i++) {

    /** A STATEMENT **/
    printf("line: %d, arg h: %Le\n", 1, (long double)fabs(l_h - h));
    temp1 = i * h;
    l_temp1 = i * l_h; // shadow
    printf("line: %d, result temp1: %Le\n", 1, (long double)fabs(l_temp1 - temp1));

    /** A STATEMENT **/
    printf("line: %d, arg1 temp1: %Le\n", 2, (long double)fabs(l_temp1 - temp1));
    t2 = fun(temp1);
    l_t2 = fun(l_temp1); // shadow
    printf("line: %d, result t2: %Le\n", 2, (long double)fabs(l_t2 - t2));

    /*** A STATEMENT ***/
    printf("line: %d, arg1 t2: %Le\n", 3, (long double)fabs(l_t2 - t2));
    printf("line: %d, arg2 t1: %Le\n", 3, (long double)fabs(l_t1 - t1));
    temp2 = t2 - t1;
    l_temp2 = l_t2 - l_t1; // shadow
    printf("line: %d, result temp2: %Le\n", 3, (long double)fabs(l_temp2 - temp2));

    /*** A STATEMENT ***/
    printf("line: %d, arg temp2: %Le\n", 4, (long double)fabs(l_temp2 - temp2));
    temp3 = temp2 * temp2;
    l_temp3 = l_temp2 * l_temp2; // shadow
    printf("line: %d, result temp3: %Le\n", 4, (long double)fabs(l_temp3 - temp3));

    /*** A STATEMENT ***/
    printf("line: %d, arg h: %Le\n", 5, (long double)fabs(l_h - h));
    temp4 = h * h;
    l_temp4 = l_h * l_h; // shadow
    printf("line: %d, result temp4: %Le\n", 5, (long double)fabs(l_temp4 - temp4));

    /*** A STATEMENT ***/
    printf("line: %d, arg1 temp4: %Le\n", 6, (long double)fabs(l_temp4 - temp4));
    printf("line: %d, arg2 temp3: %Le\n", 6, (long double)fabs(l_temp3 - temp3));
    temp5 = temp4 + temp3;
    l_temp5 = l_temp4 + l_temp3; // shadow
    printf("line: %d, result temp5: %Le\n", 6, (long double)fabs(l_temp5 - temp5));

    /*** A STATEMENT ***/
    printf("line: %d, arg temp5: %Le\n", 7, (long double)fabs(l_temp5 - temp5));
    temp6 = sqrt(temp5);
    l_temp6 = sqrt(l_temp5); // shadow
    printf("line: %d, result temp6: %Le\n", 7, (long double)fabs(l_temp6 - temp6));

    /*** A STATEMENT ****/
    printf("line: %d, arg1 s1: %Le\n", 8, (long double)fabs(l_s1 - s1));
    printf("line: %d, arg2 temp6: %Le\n", 8, (long double)fabs(l_temp6 - temp6));
      
    s1 = s1 + temp6;
    l_s1 = l_s1 + l_temp6; // shadow
    printf("line: %d, result s1: %Le\n", 8, (long double)fabs(l_s1 - s1));

    t1 = t2;
    l_t1 = l_t2; // shadow
  }

  printf(" => total error: %d %1.15Le %1.15Le\n", (long double) fabs(ans-s1) > threshold, (long double) fabs(ans-s1), 
	 (long double)threshold);
  printf(" => ans: %1.15Le\n =>  s1: %1.15Le\n", (long double)ans, (long double)s1);

  return 0;

}


