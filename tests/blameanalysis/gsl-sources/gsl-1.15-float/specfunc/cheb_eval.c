
static inline int
cheb_eval_e(const cheb_series * cs,
            const float x,
            gsl_sf_result * result)
{
  int j;
  float d  = 0.0;
  float dd = 0.0;

  float y  = (2.0*x - cs->a - cs->b) / (cs->b - cs->a);
  float y2 = 2.0 * y;

  float e = 0.0;

  for(j = cs->order; j>=1; j--) {
    float temp = d;
    d = y2*d - dd + cs->c[j];
    e += fabsf(y2*temp) + fabsf(dd) + fabsf(cs->c[j]);
    dd = temp;
  }

  { 
    float temp = d;
    d = y*d - dd + 0.5 * cs->c[0];
    e += fabsf(y*temp) + fabsf(dd) + 0.5 * fabsf(cs->c[0]);
  }

  result->val = d;
  result->err = GSL_DBL_EPSILON * e + fabsf(cs->c[cs->order]);

  return GSL_SUCCESS;
}

