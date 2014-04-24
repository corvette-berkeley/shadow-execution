#include "../../../../precimonious/logging/cov_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_cdf.h>

int main (int argc, char** argv) {

  FILE *infile = fopen(argv[1], "r");
  if (!infile) {
    printf("Could not open input file.\n");
  }

  double P, Q;
  double x, y;

  char *sx = malloc(10);
  if (!feof(infile)) {
    fscanf(infile,"%s",sx);
    x = (float)cov_deserialize(sx,10);
  }

  P = gsl_cdf_ugaussian_P (x);
//  printf ("prob(x < %f) = %f\n", x, P);

  Q = gsl_cdf_ugaussian_Q (x);
//  printf ("prob(x > %f) = %f\n", x, Q);

  x = gsl_cdf_ugaussian_Pinv (P);
//  printf ("Pinv(%f) = %f\n", P, x);

  y = gsl_cdf_ugaussian_Qinv (Q);
//  printf ("Qinv(%f) = %f\n", Q, x);

  x = x + 0;

  printf("x = %.10f\n", x);
  printf("y = %.10f\n", y);

  return 0;
}
