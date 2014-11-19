//#include "../../../logging/cov_log.h"
// #include "../../../logging/cov_checker.h"

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

int
main (void)
{
  int i; double data[2*128];

//  long double log[128];

  for (i = 0; i < 128; i++)
    {
       REAL(data,i) = 0.0; IMAG(data,i) = 0.0;
    }

  REAL(data,0) = 1.0;

  for (i = 1; i <= 10; i++)
    {
       REAL(data,i) = REAL(data,128-i) = 1.0;
    }

  for (i = 0; i < 128; i++)
    {
      printf ("%d %e %e\n", i, 
              REAL(data,i), IMAG(data,i));
    }
  printf ("\n");

  gsl_fft_complex_radix2_forward (data, 1, 128);

  int j = 0;
  for (i = 0; i < 128; i++)
    {
      printf ("%d %e %e\n", i, 
              REAL(data,i)/sqrt(128), 
              IMAG(data,i)/sqrt(128));

      // for logging
//      log[j++] = (long double)(REAL(data,i)/sqrt(128));
//      log[j++] = (long double)(IMAG(data,i)/sqrt(128));
    }

  double res = REAL(data, 127); 
  res = res + 0;

  //cov_arr_spec_log("spec.cov", 0, 256, &log[0]);
  // cov_arr_log(&log[0], 256, "result", "log.cov");
  // cov_check("log.cov", "spec.cov", 256);

  return 0;
}
