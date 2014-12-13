#include "../../logging/cov_checker.h"
#include "../../logging/cov_log.h"
#include "../../logging/cov_rand.h"
#include "../../logging/cov_serializer.h"

#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <time.h>
#include <stdarg.h>
#include <inttypes.h>

#include "demo_fn.h"

extern uint64_t current_time_ns(void);
#define ITER 1000000

#define INPUTS 10

int main (void) {

  // variables for logging/checking
  long double log[10];
  double epsilon = -6.0;
  long double threshold = 0.0;
  
  // variables for timing measurement
  uint64_t start, end;
  long int diff = 0;
  
  // reading multiple inputs from file
  int i, j, k;
  char *sa = malloc(10);
  char *sb = malloc(10);
  char *sc = malloc(10);
  float finputs[INPUTS*3];

  FILE* infile = fopen("final_inputs", "r");
  if (!infile) {
    printf("Could not open the file\n");
  }

  ///////////////////////////////////////
  // for each input set
  k = 0;
  for (i = 0; i < INPUTS; i++) {
    if (!feof(infile)) {
      
      fscanf(infile,"%s", sa);
      fscanf(infile,"%s", sb);
      fscanf(infile,"%s", sc); 
      
      finputs[k++] = (float)cov_deserialize(sa, 10);
      finputs[k++] = (float)cov_deserialize(sb, 10);
      finputs[k++] = (float)cov_deserialize(sc, 10);
    }
  }
    
  // local variables
  double x;
  double a, b, c;
  int status;
  int iter = 0, max_iter = 100;
  const gsl_root_fdfsolver_type *T;
  gsl_root_fdfsolver *s;
  double x0;


  ///////////////////////////////////////
  // repeat on input sets ITER times

  k = 0; 
  for (i = 0; i < INPUTS; i++) {

    a = finputs[k++];
    b = finputs[k++];
    c = finputs[k++];

    // timer on
    start = current_time_ns();    

    for(j = 0; j < ITER; j++) {
  
      iter = 0; max_iter = 100;
      x0 = 0.0f;
      x = (float)5.0;

      gsl_function_fdf FDF;	
      struct quadratic_params params = {a, b, -c};

      T = gsl_root_fdfsolver_newton;
      s = gsl_root_fdfsolver_alloc (T);

      FDF.f = &quadratic;
      FDF.df = &quadratic_deriv;
      FDF.fdf = &quadratic_fdf;
      FDF.params = &params;
      
      gsl_root_fdfsolver_set (s, &FDF, x);
	
      do
      {
        iter++;
        status = gsl_root_fdfsolver_iterate (s);
        x0 = x;
        x = gsl_root_fdfsolver_root (s);
        status = gsl_root_test_delta (x, x0, 0, 1e-3);
      }
      while (status == GSL_CONTINUE && iter < max_iter);

      x = x + 0; // for blame analysis

      gsl_root_fdfsolver_free (s);
    }
     
    // timer off
    end = current_time_ns();
    diff += (long int)(end-start);

    // 1. logging result and calculating threshold in only first iteration
    log[i] = (long double) x;
    if (pow(10.0, epsilon)*x > threshold) {
      threshold = (long double) pow(10.0, epsilon)*x;
    }
  }

  // 2. creating spec, or checking results
//  cov_arr_spec_log("spec.cov", threshold, INPUTS, log);
   cov_arr_log(log, INPUTS, "result", "log.cov");
   cov_check("log.cov", "spec.cov", INPUTS);
  
  // 3. print score (diff) to a file
  FILE* file;
  file = fopen("score.cov", "w");
  fprintf(file, "%ld\n", diff);
  fclose(file);
  
  fclose(infile);
  return 0;
}
