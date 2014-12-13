#include "../logging/cov_checker.h"
#include "../logging/cov_log.h"
#include "../logging/cov_rand.h"
#include "../logging/cov_serializer.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_math.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdlib.h>

extern uint64_t current_time_ns(void);
#define ITER 2000000
// #define ITER 1

int main () {

  // variables for logging/checking
//  long double log[10]; // 5 inputs in final_inputs, 2 results per input
//  double epsilon = -4.0;
//  long double threshold = 0.0;

  // variables for timing measurement
//  uint64_t start, end;
//  long int diff = 0;

  // reading multiple inputs from file
  int i, j, k = 0;
  char *sx = malloc(10);

  FILE* infile = fopen("final_inputs", "r");
  if (!infile) {
    printf("Could not open the file\n");
  }

  ///////////////////////////////////////
  // for each input set
  for (i = 0; i < 5; i++) {

    double P, Q;
    double y = 0.0;
    double x;

    // reading input values
    if (!feof(infile)) {

      fscanf(infile,"%s", sx);
      float tmp = (float) cov_deserialize(sx, 10);
      x = tmp;

      // timer on
//      start = current_time_ns();
      for(j = 0; j < ITER; j++) {

        P = gsl_cdf_ugaussian_P (x);
        //printf ("prob(x < %f) = %f\n", x, P);
	
        Q = gsl_cdf_ugaussian_Q (x);
        //printf ("prob(x > %f) = %f\n", x, Q);
	
        x = gsl_cdf_ugaussian_Pinv (P);
        //printf ("Pinv(%f) = %f\n", P, x);

        y = gsl_cdf_ugaussian_Qinv (Q);
        //printf ("Qinv(%f) = %f\n", Q, x);
      }

      // timer off
//      end = current_time_ns();
//      diff += (long int)(end-start);

      // 1. logging result and calculating threshold
//      log[k++] = (long double) x;
//      log[k++] = (long double) y;

//      if (pow(10.0, epsilon)*x > threshold) {
//        threshold = (long double) pow(10.0, epsilon)*x;
//      }
//      if (pow(10.0, epsilon)*y > threshold) {
//        threshold = (long double) pow(10.0, epsilon)*y;
//      }
      
    } // if !eof
  } // for loop for each input

  // 2. creating spec, or checking results
//  cov_arr_spec_log("spec.cov", threshold, 10, log);
//  cov_arr_log(log, 10, "result", "log.cov");
//  cov_check("log.cov", "spec.cov", 10);

  // 3. print score (diff) to a file
//  FILE* file;
//  file = fopen("score.cov", "w");
//  fprintf(file, "%ld\n", diff);
//  fclose(file);

  fclose(infile);
  return 0;
}
