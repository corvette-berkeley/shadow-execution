#include "../../../../corvette/logging/cov_serializer.h"

#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#include "demo_fn.h"

int main(int argc, char* argv[]) {

  // reading input
  //
  FILE *infile = fopen(argv[1],"r");
  if (!infile) {
    printf("Could not open input file\n");
  }

  double a;
  double b;
  double c;

  char *sa = malloc(10);
  char *sb = malloc(10);
  char *sc = malloc(10);
  if (!feof(infile)) {    
    fscanf(infile,"%s", sa);
    fscanf(infile,"%s", sb);
    fscanf(infile,"%s", sc); 
    
    a = (float)cov_deserialize(sa, 10); // float?
    b = (float)cov_deserialize(sb, 10);
    c = (float)cov_deserialize(sc, 10);
  }

  printf("here a: %f, b: %f, c: %f\n", a, b, c);
  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fdfsolver_type *T;
  gsl_root_fdfsolver *s;
  double x0, x = 5.0, r_expected = sqrt (5.0);
  gsl_function_fdf FDF;
  struct quadratic_params params = {a, b, -c};

  printf("here1.5\n");
  FDF.f = &quadratic;
  FDF.df = &quadratic_deriv;
  FDF.fdf = &quadratic_fdf;
  FDF.params = NULL;

  printf("here2\n");
  T = gsl_root_fdfsolver_newton;
  printf("here3\n");
  s = gsl_root_fdfsolver_alloc (T);
  printf("here4\n");
  gsl_root_fdfsolver_set (s, &FDF, x);
  printf("here5\n");
  /*
  printf ("using %s method\n", 
          gsl_root_fdfsolver_name (s));

  printf ("%-5s %10s %10s %10s\n",
          "iter", "root", "err", "err(est)");
  */
  do
    {
      iter++;
      status = gsl_root_fdfsolver_iterate (s);
      x0 = x;
      x = gsl_root_fdfsolver_root (s);
      status = gsl_root_test_delta (x, x0, 0, 1e-3);
      /*
      if (status == GSL_SUCCESS)
        printf ("Converged:\n");

      printf ("%5d %10.7f %+10.7f %10.7f\n",
              iter, x, x - r_expected, x - x0);
      */
    }
  while (status == GSL_CONTINUE && iter < max_iter);

  gsl_root_fdfsolver_free (s);
  // result
  x = x + 0.0;
  fclose(infile);
  return 0;
}
