#include "../../../../corvette/logging/cov_serializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <gsl/gsl_sf_bessel.h>


int main (int argc, char *argv[]) {

  double x, y, result;

  FILE* infile = fopen(argv[1], "r");
  if (!infile) {
    printf("Could not open input file\n");
  }

  char* sx = malloc(100);

  if (!feof(infile)) {
    fscanf(infile, "%s", sx);
    x = (double) cov_deserialize(sx, 10);
    y = gsl_sf_bessel_J0 (x);
    result = y + 0.0;
  }

  printf ("J0(%g) = %.18e\n", x, y);

  fclose(infile);
  return 0;
}
