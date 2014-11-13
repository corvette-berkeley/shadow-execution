// #include "../../../logging/cov_log.h"
// #include "../../../logging/cov_checker.h"

#include <stdio.h>
#include <gsl/gsl_sf_bessel.h>

int main () {
  double x = 5.0;

  double y = gsl_sf_bessel_J0 (x);

  y = y + 0;  

  //cov_spec_log("spec.cov", 0, 1, (long double) y);
  // cov_log("result", "log.cov", 1, (long double) y);
  // cov_check("log.cov", "spec.cov", 1);

  printf ("J0(%g) = %.18e\n", x, y);
  return 0;
}
