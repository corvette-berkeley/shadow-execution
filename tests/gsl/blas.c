#include "../../../logging/cov_log.h"
#include "../../../logging/cov_checker.h"
#include <stdio.h>
#include <gsl/gsl_blas.h>

int main (void)
{
  double a[6]; 
  double b[6];
  double c[4];
  
  a[0] = 0.11;
  a[1] = 0.12;
  a[2] = 0.13;
  a[3] = 0.21;
  a[4] = 0.22;
  a[5] = 0.23;

  b[0] = 1011;
  b[1] = 1012;
  b[2] = 1021;
  b[3] = 1022;
  b[4] = 1031;
  b[5] = 1032;

  c[0] = 0.00;
  c[1] = 0.00;
  c[2] = 0.00;
  c[3] = 0.00;
  
  gsl_matrix_view A = gsl_matrix_view_array(a, 2, 3);
  gsl_matrix_view B = gsl_matrix_view_array(b, 3, 2);
  gsl_matrix_view C = gsl_matrix_view_array(c, 2, 2);

  /* Compute C = A B */

  gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
                  1.0, &A.matrix, &B.matrix,
                  0.0, &C.matrix);

  printf ("[ %g, %g\n", c[0], c[1]);
  printf ("  %g, %g ]\n", c[2], c[3]);

  return 0;  
}
