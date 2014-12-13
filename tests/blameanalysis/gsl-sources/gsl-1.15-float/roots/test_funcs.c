/* roots/test_funcs.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Reid Priedhorsky, Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <config.h>
#include <math.h>
#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#include "test.h"

gsl_function create_function (float (*f)(float, void *)) 
{
  gsl_function F ;
  F.function = f;
  F.params = 0;
  return F ;
}

gsl_function_fdf create_fdf (float (*f)(float, void *),
                             float (*df)(float, void *),
                             void (*fdf)(float, void *, float *, float *))
{
  gsl_function_fdf FDF ;
  FDF.f = f ;
  FDF.df = df ;
  FDF.fdf = fdf ;
  FDF.params = 0 ;
  return FDF ;
}

/* f(x) = x^{20} - 1 */
/* f'(x) = 20x^{19} */
/* zero at x = 1 or -1 */

float
func1 (float x, void *p)
{
  return pow (x, 20.0) - 1;
}

float
func1_df (float x, void * p)
{
  return 20.0 * pow (x, 19.0);
}

void
func1_fdf (float x, void * p, float *y, float *yprime)
{
  *y = func1 (x, p);
  *yprime = 20.0 * pow (x, 19.0);
}

/* f(x) = sqrt(abs(x))*sgn(x) */
/* f'(x) = 1 / sqrt(abs(x) */
/* zero at x = 0 */
float
func2 (float x, void * p)
{
  float delta;

  if (x > 0)
    delta = 1.0;
  else if (x < 0)
    delta = -1.0;
  else
    delta = 0.0;

  return sqrt (fabs (x)) * delta;
}

float
func2_df (float x, void * p)
{
  return 1 / sqrt (fabs (x));
}

void
func2_fdf (float x, void * p, float *y, float *yprime)
{
  *y = func2 (x, p);
  *yprime = 1 / sqrt (fabs (x));
}


/* f(x) = x^2 - 1e-8 */
/* f'(x) = 2x */
/* zero at x = sqrt(1e-8) or -sqrt(1e-8) */
float
func3 (float x, void * p)
{
  return pow (x, 2.0) - 1e-8;
}

float
func3_df (float x, void * p)
{
  return 2 * x;
}

void
func3_fdf (float x, void * p, float *y, float *yprime)
{
  *y = func3 (x, p);
  *yprime = 2 * x;
}

/* f(x) = x exp(-x) */
/* f'(x) = exp(-x) - x exp(-x) */
/* zero at x = 0 */
float
func4 (float x, void * p)
{
  return x * exp (-x);
}

float
func4_df (float x, void * p)
{
  return exp (-x) - x * exp (-x);
}

void
func4_fdf (float x, void * p, float *y, float *yprime)
{
  *y = func4 (x, p);
  *yprime = exp (-x) - x * exp (-x);
}

/* f(x) = 1/(1+exp(x)) */
/* f'(x) = -exp(x) / (1 + exp(x))^2 */
/* no roots! */
float
func5 (float x, void * p)
{
  return 1 / (1 + exp (x));
}

float
func5_df (float x, void * p)
{
  return -exp (x) / pow (1 + exp (x), 2.0);
}

void
func5_fdf (float x, void * p, float *y, float *yprime)
{
  *y = func5 (x, p);
  *yprime = -exp (x) / pow (1 + exp (x), 2.0);
}

/* f(x) = (x - 1)^7 */
/* f'(x) = 7 * (x - 1)^6 */
/* zero at x = 1 */
float
func6 (float x, void * p)
{
  return pow (x - 1, 7.0);
}

float
func6_df (float x, void * p)
{
  return 7.0 * pow (x - 1, 6.0);
}

void
func6_fdf (float x, void * p, float *y, float *yprime)
{
  *y = func6 (x, p);
  *yprime = 7.0 * pow (x - 1, 6.0);
}

/* sin(x) packaged up nicely. */
float
sin_f (float x, void * p)
{
  return sin (x);
}

float
sin_df (float x, void * p)
{
  return cos (x);
}

void
sin_fdf (float x, void * p, float *y, float *yprime)
{
  *y = sin (x);
  *yprime = cos (x);
}

/* cos(x) packaged up nicely. */
float
cos_f (float x, void * p)
{
  return cos (x);
}

float
cos_df (float x, void * p)
{
  return -sin (x);
}

void
cos_fdf (float x, void * p, float *y, float *yprime)
{
  *y = cos (x);
  *yprime = -sin (x);
}
