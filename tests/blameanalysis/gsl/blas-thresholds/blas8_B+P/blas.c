#include "../../../../logging/cov_log.h"
#include "../../../../logging/cov_checker.h"
#include "../../../../logging/cov_rand.h"
#include "../../../../logging/cov_serializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>
#include <math.h>
#include <gsl/gsl_blas.h>

#define INPUT_SIZE 1
#define ITERS 200000
extern uint64_t current_time_ns(void);

int main (void)
{
  uint64_t start, end;
  long int diff = 0;
  long double results[400*INPUT_SIZE];
  double epsilon = -8.0;
  long double threshold;

  double a[600]; 
  double b[600];
  double c[400];

  float a_max = 0.0;
  float b_max = 0.0;
  float c_max = 0.0;

  int i, j;

  FILE* infile = fopen("final_inputs", "r");
  if (!infile)
  {
    printf("Could not open final_inputs\n");
    exit(0);
  }

  for (i = 0; i < INPUT_SIZE; i++)
  {
    //
    // read inputs from final_inputs
    //
    if (!feof(infile))
    {
      for (j = 0; j < 1200; j++) {
        char* s = malloc(100);
        fscanf(infile, "%s", s);
        if (j < 600)
        {
          a[j] = (float) cov_deserialize(s, 10);
          if (a[j] > a_max) 
          {
            a_max = a[j];
          }
        }
        else 
        {
          b[j-600] = (float) cov_deserialize(s, 10);
          if (b[j-600] > b_max)
          {
            b_max = b[j-600];
          }
        }
      }
    }

    for (i = 0; i < 400; i++)
    {
      c[i] = 0.00;
    }

    //
    // run program; record timing as score
    //
    start = current_time_ns();
    for (j = 0; j < ITERS; j++) {
      gsl_matrix_view A = gsl_matrix_view_array(a, 20, 30);
      gsl_matrix_view B = gsl_matrix_view_array(b, 30, 20);
      gsl_matrix_view C = gsl_matrix_view_array(c, 20, 20);

      /* Compute C = A B */

      gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
          1.0, &A.matrix, &B.matrix,
          0.0, &C.matrix);
    }
    end = current_time_ns();
    diff += (long int)(end-start);

    for (i = 0; i < 400; i++)
    {
      if (c[i] > c_max)
      {
        c_max = c[i];
      }
    }
  }

  threshold = (long double) pow(10.0, epsilon)*a_max*b_max;

  //cov_spec_log("spec.cov", threshold, 1, (long double) c_max);
  cov_log("result", "log.cov", 1, (long double) c_max);
  cov_check("log.cov", "spec.cov", 1);

  fclose(infile);

  //
  // print score (diff) to score.cov
  //
  FILE* file;
  file = fopen("score.cov", "w");
  fprintf(file, "%ld\n", diff);
  fclose(file);

  return 0;  
}
