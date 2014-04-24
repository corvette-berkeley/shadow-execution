#include "../../../../precimonious/logging/cov_serializer.h"

#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#include "demo_fn.h"

int main (int argc, char *argv[]) {
  //
  // reading input
  //
  FILE *infile = fopen(argv[1],"r");
  if (!infile) {
    printf("Could not open input file\n");
  }

  char *sa = malloc(10);
  char *sb = malloc(10);
  char *sc = malloc(10);
  double a, b, c;

  if (!feof(infile)) {
    fscanf(infile, "%s", sa);
    fscanf(infile, "%s", sb);
    fscanf(infile, "%s", sc);
    a = (float)cov_deserialize(sa, 10);
    b = (float)cov_deserialize(sb, 10);
    c = (float)cov_deserialize(sc, 10);
  }

  //
  // main computation
  //
  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fsolver_type *T;
  gsl_root_fsolver *s;
  double r = 0;
  double x_lo = 0.0, x_hi = 5.0;
  gsl_function F;
  struct quadratic_params params = {a, b, -c};

  F.function = &quadratic;
  F.params = &params;

  T = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc (T);
  gsl_root_fsolver_set (s, &F, x_lo, x_hi);

  do
  {
    iter++;
    status = gsl_root_fsolver_iterate (s);
    r = gsl_root_fsolver_root (s);
    x_lo = gsl_root_fsolver_x_lower (s);
    x_hi = gsl_root_fsolver_x_upper (s);
    status = gsl_root_test_interval (x_lo, x_hi,
        0, 0.001);
  }
  while (status == GSL_CONTINUE && iter < max_iter);

  gsl_root_fsolver_free (s);

  r = r + 0;

  printf("r = %.10f\n", r);

  return status;
}
