// #include "../../../logging/cov_log.h"
// #include "../../../logging/cov_checker.h"
#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sum.h>

#define N 20

int
main (void)
{
  double t[N];
  double sum_accel, err;
  double sum = 0;
  int n;
  
  gsl_sum_levin_u_workspace * w 
    = gsl_sum_levin_u_alloc (N);

  const double zeta_2 = M_PI * M_PI / 6.0;
  
  /* terms for zeta(2) = \sum_{n=1}^{\infty} 1/n^2 */

  for (n = 0; n < N; n++)
    {
      double np1 = n + 1.0;
      t[n] = 1.0 / (np1 * np1);
      sum += t[n];
    }
  
  gsl_sum_levin_u_accel (t, N, w, &sum_accel, &err);

  printf ("term-by-term sum = % .16f using %d terms\n", 
          sum, N);

  printf ("term-by-term sum = % .16f using %d terms\n", 
          w->sum_plain, w->terms_used);

  printf ("exact value      = % .16f\n", zeta_2);
  printf ("accelerated sum  = % .16f using %d terms\n", 
          sum_accel, w->terms_used);

  printf ("estimated error  = % .16f\n", err);
  printf ("actual error     = % .16f\n", 
          sum_accel - zeta_2);

  gsl_sum_levin_u_free (w);

  double res = w->sum_plain;
  res = res + 0;

  //cov_spec_log("spec.cov", 0, 4, (long double) sum, (long double) w->sum_plain, (long double) sum_accel, (long double) err);
//  cov_log("result", "log.cov", 4, (long double) sum, (long double) w->sum_plain, (long double) sum_accel, (long double) err);
//  cov_check("log.cov", "spec.cov", 4);
  return 0;
}
