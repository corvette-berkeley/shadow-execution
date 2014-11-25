//#include "../logging/cov_checker.h"
//#include "../logging/cov_log.h"
//#include "../logging/cov_rand.h"
//#include "../logging/cov_serializer.h"

#include <stdio.h>
#include <inttypes.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#include "demo_fn.h"

#define ITER 5000000

int main () {
  int it = 0;

//  uint64_t start, end, diff;
  float r = 0; 
  float r_expected = sqrt (5.0);
//  diff = 0;

  for (it = 0; it < ITER; it++) {
    r = 0;
    int status;
    int iter = 0, max_iter = 100;
    const gsl_root_fsolver_type *T;
    gsl_root_fsolver *s;
    float x_lo = 0.0, x_hi = 5.0;
    gsl_function F;
    struct quadratic_params params = {1.0, 0.0, -5.0};

//    start = current_time_ns();

    F.function = &quadratic;
    F.params = &params;

    T = gsl_root_fsolver_brent;
    s = gsl_root_fsolver_alloc (T);

    gsl_root_fsolver_set (s, &F, x_lo, x_hi);

    //  printf ("using %s method\n",i 
    //          gsl_root_fsolver_name (s));

    //  printf ("%5s [%9s, %9s] %9s %10s %9s\n",
    //          "iter", "lower", "upper", "root", 
    //          "err", "err(est)");

    do
    {
      iter++;
      status = gsl_root_fsolver_iterate (s);
      r = gsl_root_fsolver_root (s);
      x_lo = gsl_root_fsolver_x_lower (s);
      x_hi = gsl_root_fsolver_x_upper (s);
      status = gsl_root_test_interval (x_lo, x_hi,
          0, 0.001);

      //      if (status == GSL_SUCCESS)
      //        printf ("Converged:\n");

      //      printf ("%5d [%.7f, %.7f] %.7f %+.7f %.7f\n",
      //              iter, x_lo, x_hi,
      //              r, r - r_expected, 
      //              x_hi - x_lo);

    }
    while (status == GSL_CONTINUE && iter < max_iter);

    gsl_root_fsolver_free (s);

    r = r + 0;
//    end = current_time_ns();
//    diff += end - start;
  }

//  printf("Time %ld\n", diff/1000000);

//  float epsilon = -4.0;
//  long float threshold = pow(10.0, epsilon) * r;

  // 1. creating spec, or checking results
//  cov_spec_log("spec.cov", threshold, 1, (long float) r);
//  cov_log("result", "log.cov", 1, (long float) r);
//  cov_check("log.cov", "spec.cov", 1);

  // 2. print store (diff) to a file
//  FILE* file = fopen("score.cov", "w");
//  fprintf(file, "%ld\n", diff);
//  fclose(file);

  return 0;
}
