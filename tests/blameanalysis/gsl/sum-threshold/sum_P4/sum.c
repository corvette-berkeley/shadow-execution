#include "../../logging/cov_log.h"
#include "../../logging/cov_checker.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>
#include <stdarg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sum.h>

extern uint64_t current_time_ns(void);

#define ITERS 100000

int main (void) {
  uint64_t start, end;
  long long int diff = 0;

  long double log;
  double epsilon = -4.0;
  long double threshold = 0.0;
  int i, j;

  double sum_accel, err;
  double t[100];
  int n;

  gsl_sum_levin_u_workspace * w 
    = gsl_sum_levin_u_alloc (100);

  for (n = 0; n < 100; n++) {
    float np1 = n + 1.0;
    t[n] = 1.0 / (np1 * np1);
  }


  start = current_time_us();
  // computation
  for (j = 0; j < ITERS; j++) {
    sum_accel = 0;
    err = 0;
    gsl_sum_levin_u_accel (t, 100, w, &sum_accel, &err);

  }
  // end computation
  end = current_time_us();
  diff += (long long int) (end - start);

  sum_accel = sum_accel + 0;

  gsl_sum_levin_u_free (w);


  log = (long double) sum_accel;

  if (pow(10.0, epsilon) * sum_accel > threshold) {
    threshold = (long double) pow(10.0, epsilon) * sum_accel;
  }


//  cov_spec_log("spec.cov", threshold, 1, log);
  cov_log("result", "log.cov", 1 , log);
  cov_check("log.cov", "spec.cov", 1);

  FILE* file;
  file = fopen("score.cov", "w");
  fprintf(file, "%lld\n", diff);
  fclose(file);

  return 0;
}
