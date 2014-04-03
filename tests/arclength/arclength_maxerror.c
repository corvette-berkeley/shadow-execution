#include <math.h>
#include <stdio.h>

#include <map>

using namespace std;
map<int, long double> max_errors;

double fun(double x){

  int k, n = 5;
  double t1, d1 = 1.0L;
  double temp1, temp2, temp3;

  long double l_t1, l_d1 = 1.0L;
  long double l_temp1, l_temp2, l_temp3;
  long double l_x = (long double)x;
  long double current_error;
  int stmt;

  /** STATEMENT 1**/
  stmt = 1;
  t1 = x;
  l_t1 = l_x; // shadow 1
  current_error = (long double)fabs(l_t1 - t1);
  if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
  printf("fun stmt: %d, t1: %Le\n", stmt, current_error);
  
  for(k = 1; k <= n; k++) {

    /** STATEMENT 2**/
    stmt = 2;
    printf("fun stmt: %d, arg d1: %Le\n", stmt, (long double)fabs(l_d1 - d1));
    d1 = 2.0 * d1;
    l_d1 = 2.0 * l_d1; // shadow
    current_error = (long double)fabs(l_d1 - d1);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("fun stmt: %d, d1: %Le\n", stmt, current_error);

    /** STATEMENT 3**/
    stmt = 3;
    printf("fun stmt: %d, arg1 d1: %Le\n", stmt, (long double)fabs(l_d1 - d1));
    printf("fun stmt: %d, arg2 x: %Le\n", stmt, (long double)fabs(l_x - x));
    temp1 = d1 * x;
    l_temp1 = l_d1 * l_x; // shadow
    current_error = (long double)fabs(l_temp1 - temp1);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("fun stmt: %d, temp1: %Le\n", stmt, current_error);

    /** STATEMENT 4**/
    stmt = 4;
    printf("fun stmt: %d, arg temp1: %Le\n", stmt, (long double)fabs(l_temp1 - temp1));
    temp2 = sin(temp1);
    l_temp2 = sin(l_temp1); // shadow
    current_error = (long double)fabs(l_temp2 - temp2);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("fun stmt: %d, temp2: %Le\n", stmt, current_error);

    /** STATEMENT 5**/
    stmt = 5;
    printf("fun stmt: %d, arg1 temp2: %Le\n", stmt, (long double)fabs(l_temp2 - temp2));
    printf("fun stmt: %d, arg2 d1: %Le\n", stmt, (long double)fabs(l_d1 - d1));
    temp3 = temp2 / d1;
    l_temp3 = l_temp2 / l_d1; // shadow
    current_error = (long double)fabs(l_temp3 - temp3);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("fun stmt: %d, temp3: %Le\n", stmt, current_error);

    /** STATEMENT 6**/
    stmt = 6;
    printf("fun stmt: %d, arg1 t1: %Le\n", stmt, (long double)fabs(l_t1 - t1));
    printf("fun stmt: %d, arg2 temp3: %Le\n", stmt, (long double)fabs(l_temp3 - temp3));
    t1 = t1 + temp3;
    l_t1 = l_t1 + l_temp3; // shadow
    current_error =  (long double)fabs(l_t1 - t1);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("fun stmt: %d, t1: %Le\n", stmt, current_error);
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
  long double current_error;
  int stmt;

  // initializing map
  int stmts = 20;
  for(int i = 0; i < stmts; i++) {
    max_errors[i] = 0.0L;
  }

  /** STATEMENT 7 **/
  stmt = 7;
  t1 = -1.0;
  l_t1 = -1.0; // shadow
  max_errors[stmt] = 0.0;

  /** STATEMENT 8 **/
  stmt = 8;
  dppi = acos(t1);
  l_dppi = acos(l_t1); // shadow
  current_error = (long double)fabs(l_dppi - dppi);
  if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
  printf("main stmt: %d, dppi: %Le\n", stmt, current_error);

  /** STATEMENT 9 **/
  stmt = 9;
  s1 = 0.0;
  l_s1 = 0.0; // shadow
  max_errors[stmt] = 0.0;

  /** STATEMENT 10 **/
  stmt = 10;
  t1 = 0.0;
  l_t1 = 0.0; // shadow
  max_errors[stmt] = 0.0;

  /** STATEMENT 11**/
  stmt = 11;
  printf("stmt: %d, arg dppi: %Le\n", stmt, (long double)fabs(l_dppi - dppi));
  h = dppi / n;
  l_h = l_dppi / n; // shadow
  current_error = (long double)fabs(l_h - h);
  if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
  printf("main stmt: %d, h: %Le\n", stmt, current_error);

  for(i = 1; i <= n; i++) {

    /** STATEMENT 12**/
    stmt = 12;
    printf("stmt: %d, arg h: %Le\n", stmt, (long double)fabs(l_h - h));
    temp1 = i * h;
    l_temp1 = i * l_h; // shadow
    current_error = (long double)fabs(l_temp1 - temp1);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, temp1: %Le\n", stmt, current_error);

    /** STATEMENT 13**/
    stmt = 13;
    printf("stmt: %d, arg1 temp1: %Le\n", stmt, (long double)fabs(l_temp1 - temp1));
    t2 = fun(temp1);
    l_t2 = fun(l_temp1); // shadow
    current_error = (long double)fabs(l_t2 - t2);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, t2: %Le\n", stmt, current_error);

    /*** STATEMENT 14***/
    stmt = 14;
    printf("stmt: %d, arg1 t2: %Le\n", stmt, (long double)fabs(l_t2 - t2));
    printf("stmt: %d, arg2 t1: %Le\n", stmt, (long double)fabs(l_t1 - t1));
    temp2 = t2 - t1;
    l_temp2 = l_t2 - l_t1; // shadow
    current_error =  (long double)fabs(l_temp2 - temp2);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, temp2: %Le\n", stmt, current_error);

    /*** STATEMENT 15***/
    stmt = 15;
    printf("stmt: %d, arg temp2: %Le\n", stmt, (long double)fabs(l_temp2 - temp2));
    temp3 = temp2 * temp2;
    l_temp3 = l_temp2 * l_temp2; // shadow
    current_error = (long double)fabs(l_temp3 - temp3);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, temp3: %Le\n", stmt, current_error);

    /*** STATEMENT 16***/
    stmt = 16;
    printf("stmt: %d, arg h: %Le\n", stmt, (long double)fabs(l_h - h));
    temp4 = h * h;
    l_temp4 = l_h * l_h; // shadow
    current_error = (long double)fabs(l_temp4 - temp4);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, temp4: %Le\n", stmt, current_error);

    /*** STATEMENT 17***/
    stmt = 17;
    printf("stmt: %d, arg1 temp4: %Le\n", stmt, (long double)fabs(l_temp4 - temp4));
    printf("stmt: %d, arg2 temp3: %Le\n", stmt, (long double)fabs(l_temp3 - temp3));
    temp5 = temp4 + temp3;
    l_temp5 = l_temp4 + l_temp3; // shadow
    current_error = (long double)fabs(l_temp5 - temp5);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("stmt: %d, temp5: %Le\n", stmt, current_error);

    /*** STATEMENT 18***/
    stmt = 18;
    printf("stmt: %d, arg temp5: %Le\n", stmt, (long double)fabs(l_temp5 - temp5));
    temp6 = sqrt(temp5);
    l_temp6 = sqrt(l_temp5); // shadow
    current_error =  (long double)fabs(l_temp6 - temp6);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, temp6: %Le\n", stmt, current_error);

    /*** STATEMENT 19****/
    stmt = 19;
    printf("stmt: %d, arg1 s1: %Le\n", stmt, (long double)fabs(l_s1 - s1));
    printf("stmt: %d, arg2 temp6: %Le\n", stmt, (long double)fabs(l_temp6 - temp6));
    s1 = s1 + temp6;
    l_s1 = l_s1 + l_temp6; // shadow
    current_error = (long double)fabs(l_s1 - s1);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, s1: %Le\n", stmt, current_error);

    /** STATEMENT 20***/
    stmt = 20;
    t1 = t2;
    l_t1 = l_t2; // shadow
    current_error = (long double)fabs(l_t1 - t1);
    if (max_errors[stmt] < current_error) max_errors[stmt] = current_error;
    printf("main stmt: %d, t1: %Le\n", stmt, current_error);
  }


  // printing errors per statement
  printf("Max error per statement\n");
  for(int i = 0; i < stmts + 1; i++) {
    printf("stmt %d: max error %Le\n", i, max_errors[i]);
  }

  printf(" => total error: %d %1.15Le %1.15Le\n", (long double) fabs(ans-s1) > threshold, (long double) fabs(ans-s1), 
	 (long double)threshold);
  printf(" => ans: %1.15Le\n =>  s1: %1.15Le\n", (long double)ans, (long double)s1);

  return 0;

}


