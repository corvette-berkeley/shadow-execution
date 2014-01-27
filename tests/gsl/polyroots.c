#include <stdio.h>
#include <gsl/gsl_poly.h>

int main () {
  int i;
  /* coefficients of P(x) =  -1 + x^5  */
//  double a[6] = { -1, 0, 0, 0, 0, 1 };  
  double a[6];
  double z[10];

  a[0] = -1;
  a[1] = 0;
  a[2] = 0;
  a[3] = 0;
  a[4] = 0;
  a[5] = 1;

  gsl_poly_complex_workspace * w 
      = gsl_poly_complex_workspace_alloc (6);
  
  gsl_poly_complex_solve (a, 6, w, z);

  gsl_poly_complex_workspace_free (w);

  for (i = 0; i < 5; i++)
    {
      printf ("z%d = %+.18f %+.18f\n", 
              i, z[2*i], z[2*i+1]);
    }

  return 0;
}
