#include "../logging/cov_log.h"
#include "../logging/cov_rand.h"
#include "../logging/cov_checker.h"
#include "../logging/cov_serializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <inttypes.h>
#include <gsl/gsl_sf_bessel.h>

#define INPUT_SIZE 3
#define ITERS 5000000
uint64_t current_time_ns(void);

int main () {

  // Variables for validating purpose.
  float inputs[INPUT_SIZE];
  uint64_t start, end;
  uint64_t diff = 0;
  FILE *infile = fopen("final_inputs", "r");
  if (!infile) {
    printf("Could not open file_inputs\n");
    exit(5);
  }

  // Core computation.
  int i, j;
  double x, y;
  for (i = 0; i < INPUT_SIZE; i++) {
    char *sx = malloc(100);
    // Read inputs from final_inputs.
    if (feof(infile)) {
      printf("Reaching the end of file_inputs.\n");
      exit(5);
    }
    fscanf(infile, "%s", sx);
    inputs[i] = cov_deserialize(sx, 10);
    x = inputs[i];

    // Run program; record timing as score
    start = current_time_ns(); 
    for (j = 0; j < ITERS; j++) {
      y = gsl_sf_bessel_J0 (x);
      y = y + 0;
    }
    end = current_time_ns();
    diff += (end - start);
  }

  fclose(infile);

  // Print score (diff) to score.cov.
  FILE* file;
  file = fopen("score.cov", "w");
  fprintf(file, "%llu\n", diff);
  fclose(file);

  return 0;
}
