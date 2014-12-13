#include "../../logging/cov_log.h"
#include "../../logging/cov_serializer.h"
#include "../../logging/cov_checker.h"

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_poly.h>
#include <stdarg.h>
#include <inttypes.h>

#define INPUT_SIZE 10
#define ITERS 100000

extern uint64_t current_time_ns(void);

int main () {
  uint64_t start, end;
  long int diff = 0;
  int i, j, k;

  long double results[INPUT_SIZE*10];
  long double threshold = 0;
  double epsilon = -4.0;

  FILE* infile = fopen("final_inputs", "r");
  if (!infile) 
  {
    printf("Could not open the file\n");
    exit(0);
  }

  for (i = 0; i < INPUT_SIZE; i++)
  {
    double a[6];
    float z[10];
    char* s = malloc(10);

    if (!feof(infile))
    {
      // reading next set of values
      for (j = 0; j < 6; j++)
      {
        fscanf(infile, "%s", s);
        float tmp = cov_deserialize(s, 10);
        a[j] = tmp;
      }

      // do computation
      start = current_time_ns();
      for (j = 0; j < ITERS; j++) 
      {
        for (k = 0; k < 10; k++)
        {
          z[k] = 0;
        }
        gsl_poly_complex_workspace * w 
          = gsl_poly_complex_workspace_alloc (6);

        gsl_poly_complex_solve (a, 6, w, z);

        gsl_poly_complex_workspace_free (w);
      }
      end = current_time_ns();
      diff += (long int)(end-start);

      // log results
      double z_max = 0;
      for (j = 0; j < 10; j++)
      {
        results[i*10+j] = (long double) z[j];
        if (abs(z[j]) > z_max)
          z_max = abs(z[j]);
      }

      // compute threshold
      if (pow(10, epsilon)*z_max > threshold)
        threshold = pow(2, epsilon)*z_max;
    }
  }

  //
  // log results
  //
//  cov_arr_spec_log("spec.cov", threshold, 10*INPUT_SIZE, results);
  cov_arr_log(results, 10*INPUT_SIZE, "result", "log.cov");
  cov_check("log.cov", "spec.cov", 10*INPUT_SIZE);

  //
  // write score (diff) to score.cov
  //
  FILE* file = fopen("score.cov", "w");
  fprintf(file, "%ld", diff);
  fclose(file);

  return 0;
}
