#include "../../../../logging/cov_serializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <gsl/gsl_sf_bessel.h>


int main () {

  long double threshold;
  double x, y;
  int i;
  int j;

  FILE* infile = fopen("final_inputs", "r");
  if (!infile) {
    printf("Could not open final_inputs\n");
  }

  char* sx = malloc(100);

  if (!feof(infile)) {
    fscanf(infile, "%s", sx);
    x = (double) cov_deserialize(sx, 10);
    y = gsl_sf_bessel_J0 (x);
    double result = y + 0.0;
  }

  printf ("J0(%g) = %.18e\n", x, y);

  fclose(infile);
  return 0;
}
