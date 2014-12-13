/* roots/test.h
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

gsl_function create_function(float (*f)(float, void*));
gsl_function_fdf create_fdf(float (*f)(float, void*), float (*df)(float, void*),
							void (*fdf)(float, void*, float*, float*));

void test_macros(void);

void test_roots(void);

void test_poly(void);

void test_f(const gsl_root_fsolver_type* T, const char* description, gsl_function* f, float lower_bound,
			float upper_bound, float correct_root);

void test_f_e(const gsl_root_fsolver_type* T, const char* description, gsl_function* f, float lower_bound,
			  float upper_bound, float correct_root);

void test_fdf(const gsl_root_fdfsolver_type* T, const char* description, gsl_function_fdf* fdf, float root,
			  float correct_root);

void test_fdf_e(const gsl_root_fdfsolver_type* T, const char* description, gsl_function_fdf* fdf, float root,
				float correct_root);


void usage(void);

void error_handler(const char* reason, const char* file, int line);

float func1(float x, void* p);

float func1_df(float x, void* p);

void func1_fdf(float x, void* p, float* y, float* yprime);

float func2(float x, void* p);

float func2_df(float x, void* p);

void func2_fdf(float x, void* p, float* y, float* yprime);

float func3(float x, void* p);

float func3_df(float x, void* p);

void func3_fdf(float x, void* p, float* y, float* yprime);

float func4(float x, void* p);

float func4_df(float x, void* p);

void func4_fdf(float x, void* p, float* y, float* yprime);

float func5(float x, void* p);

float func5_df(float x, void* p);

void func5_fdf(float x, void* p, float* y, float* yprime);

float func6(float x, void* p);

float func6_df(float x, void* p);

void func6_fdf(float x, void* p, float* y, float* yprime);

float sin_f(float x, void* p);

float sin_df(float x, void* p);

void sin_fdf(float x, void* p, float* y, float* yprime);

float cos_f(float x, void* p);

float cos_df(float x, void* p);

void cos_fdf(float x, void* p, float* y, float* yprime);
