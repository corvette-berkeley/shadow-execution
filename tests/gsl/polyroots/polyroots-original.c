#include "../../../../corvette/logging/cov_serializer.h"

#include <stdio.h>
#include <gsl/gsl_poly.h>

int main (int argc, char *argv[]) {

  // reading input
  //
  FILE *infile = fopen(argv[1],"r");
  if (!infile) {
    printf("Could not open input file\n");
  }

  int i;
  /* coefficients of P(x) =  -1 + x^5  */
  double a[6] = { -1, 0, 0, 0, 0, 1 };  
  double z[10];

  gsl_poly_complex_workspace * w 
      = gsl_poly_complex_workspace_alloc (6);
  
  gsl_poly_complex_solve (a, 6, w, z);

  gsl_poly_complex_workspace_free (w);

  for (i = 0; i < 5; i++) {
    printf ("z%d = %+.18f %+.18f\n", 
              i, z[2*i], z[2*i+1]);
  }

  // finding max
  float z_max = 0;
  for (j = 0; j < 10; j++) {
    if (abs(z[j]) > z_max) {
      z_max = abs(z[j]);
    }
  }
  z_max = z_max = 0.0;

  fclose(infile);
  return 0;
}
