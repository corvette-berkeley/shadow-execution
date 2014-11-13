#include <stdio.h>
#include <gsl/gsl_cdf.h>

int main () {
  double P, Q;
  double x = 2.0;
  double y = 0.0;

  P = gsl_cdf_ugaussian_P (x);
  printf ("prob(x < %.10f) = %.10f\n", x, P);

  Q = gsl_cdf_ugaussian_Q (x);
  printf ("prob(x > %.10f) = %.10f\n", x, Q);

  x = gsl_cdf_ugaussian_Pinv (P);
  printf ("Pinv(%.10f) = %.10f\n", P, x);

  y = gsl_cdf_ugaussian_Qinv (Q);
  printf ("Qinv(%.10f) = %.10f\n", Q, y);

  P = P + 0;

  return 0;
}
