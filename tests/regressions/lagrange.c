#include <stdio.h>
#include <math.h>

int polymul(int np, double p[np], double q[1], double r[np+1]);

int matmulx(int n1, int n2, double a[n1][n2], double b[n2], double c[n1]);

double factorial(int n);

int main() {
  int i, is, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, j, k, np;
  const int n = 8;
  double a[n+1][n+1], c[n+1], d[n+1], f1, f2, p[n+1], q[n+1], r[n+1];
  
  c[0] = 5.0;
  c[1] = 2304.0;
  c[2] = 118101.0;
  c[3] = 1838336.0;
  c[4] = 14855109.0;
  c[5] = 79514880.0;
  c[6] = 321537749.0;
  c[7] = 1062287616.0;
  c[8] = 3014530821.0;
  
  for (k = 0; k <= n; k++) {
    np = 0;
    p[0] = 1.0;
    
    for (j = 0; j <= n; j++) {
      if (j != k) {
	q[0] = - (double) j;
	q[1] = 1.0;
	polymul(np, p, q, r);
	np = np + 1;
	
	for (i = 0; i <= np; i++) {
	  p[i] = r[i];
	}
      }
    }
    
    f1 = factorial(k);
    f2 = factorial(n-k);
    int temp = n-k;
    is = pow(-1,temp);
    
    for (j = 0; j <= n; j++) {
      a[j][k] = ((double) is) * p[j] / (f1 * f2);
    }
  }
    
  matmulx(n, n, a, c, d);
  return 0;
}

int polymul(int np, double p[np], double q[1], double r[np+1]) {
  // This multiplies the np-degree polynomial P by the unit-degree polynomial Q,
  // yielding the (np+1)-degree polynomial R.
  
  int i, j, k;
  
  for (k = 0; k <= np; k++) {
    r[k] = p[k]*q[0];
  }
  
  r[np+1] = 0.0;
  
  for (k = 0; k <= np; k++) {
    r[k+1] = r[k+1] + p[k] * q[1];
  }
  
  return 0;
}

int matmulx(int n1, int n2, double a[n1+1][n2+1], double b[n2], double c[n1]) {
  int i, k;
  double t1;
  
  for (i = 0; i <= n1; i++) {
    t1 = 0.0;
    for (k = 0; k <= n2; k++) {
      t1 = t1 + a[i][k]*b[k];
    }
    c[i] = t1;
  }
  
  return 0;
}

double factorial(int n) {
  int k;
  double t1, res;
  
  t1 = 1.0;
  
  for (k = 2; k <= n; k++) {
    t1 = ((double) k) * t1;
  }
  
  res = t1;
  
  return res;
}

