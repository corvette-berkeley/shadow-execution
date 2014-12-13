#ifndef LOG_H_DEMO_FN
#define LOG_H_DEMO_FN

struct quadratic_params {
	float a, b, c;
};

float quadratic(float x, void* params);
float quadratic_deriv(float x, void* params);
void quadratic_fdf(float x, void* params, float* y, float* dy);

#endif
