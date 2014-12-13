#include "demo_fn.h"

float
quadratic (float x, void *params)
{
  struct quadratic_params *p 
    = (struct quadratic_params *) params;

  float a = p->a;
  float b = p->b;
  float c = p->c;

  return (a * x + b) * x + c;
}

float
quadratic_deriv (float x, void *params)
{
  struct quadratic_params *p 
    = (struct quadratic_params *) params;

  float a = p->a;
  float b = p->b;
  float c = p->c;

  return 2.0 * a * x + b;
}

void
quadratic_fdf (float x, void *params, 
               float *y, float *dy)
{
  struct quadratic_params *p 
    = (struct quadratic_params *) params;

  float a = p->a;
  float b = p->b;
  float c = p->c;

  *y = (a * x + b) * x + c;
  *dy = 2.0 * a * x + b;
}
