#include "../../../../corvette/logging/cov_serializer.h"

#include <math.h>
#include <stdio.h>
#include <gsl/gsl_poly.h>

int main (int argc, char *argv[]) {

  // reading input
  //
  FILE *infile = fopen(argv[1],"r");
  if (!infile) {
    printf("Could not open input file\n");
  }

  int i, j;
  /* coefficients of P(x) =  -1 + x^5  */
  //  double a[6] = { -1, 0, 0, 0, 0, 1 };  
  double a[6];
  double z[10];

  char *s = malloc(10);

  for(j = 0; j < 6; j++) {
    fscanf(infile, "%s", s);
    a[j] = (float)cov_deserialize(s, 10);
  }

  gsl_poly_complex_workspace * w 
      = gsl_poly_complex_workspace_alloc (6);
  
  gsl_poly_complex_solve (a, 6, w, z);

  gsl_poly_complex_workspace_free (w);

  // second version: finding norm
  double norm = 0.0;
  for(j = 0; j < 10; j++) {
    norm += z[j]*z[j];
  }
  norm = sqrt(norm);

  fclose(infile);
  norm = norm + 0.0;
  return 0;
}
