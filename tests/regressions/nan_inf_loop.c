#include <stdio.h>
#include <float.h>

/**
 * In this program, consider a as an input. 
 * If a is zero, an inf is created during the execution
 * of the program causes the program runs into infinite loop.
 *
 * If we consider threshold as an input, if we set threshold 
 * to be inf then the program runs into infinite loop.
 */
int main() {
  float a, b, threshold;
  double c = 10e10;

  a = 10e-29;
  b = 1;

  do {
    a = a/c; 
    threshold = b/a;
  } while (threshold > DBL_MAX);
}
