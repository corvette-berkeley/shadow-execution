#include <stdio.h>
#include <math.h>

int polymul(int np, double p[np], double q[1], double r[np+1]);

int matmulx(int n1, int n2, double a[n1][n2], double b[n2], double c[n1]);

double factorial(int n);

// void aprintf(char* sre, int n, double a[n]);

int main() {
	int i, is, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, j, k, np;
	const int n = 8;
	double a[n+1][n+1], c[n+1], d[n+1], f1, f2, p[n+1], q[n+1], r[n+1];

	// succeeds on this example
	// c[0] = 1.0;
	// c[1] = 1048579.0;
	// 	c[2] = 16777489.0;
	// c[3] = 84941299.0;
	// 	c[4] = 268501249.0;
	// 	c[5] = 655751251.0;
	// 	c[6] = 1360635409.0;
	// 	c[7] = 2523398179.0;
	// 	c[8] = 4311748609.0;

	// fails on this example
	// c(0) = 1.d0;
	// c[1] = 134217731.0;
	// c[2] = 8589938753.0;
	// c[3] = 97845255883.0;
	// c[4] = 549772595201.0;
	// c[5] = 2097396156251.0;
	// c[6] = 6264239146561.0;
	// c[7] = 15804422886323.0;
	// c[8] = 35253091827713.0;
	//	c[9] = 71611233653971.0;
	//	c[10] = 135217729000001.0;
	//	c[11] = 240913322581691.0;
	//	c[12] = 409688091758593.0;

		c[0] = 5.0;
		c[1] = 2304.0;
		c[2] = 118101.0;
		c[3] = 1838336.0;
		c[4] = 14855109.0;
		c[5] = 79514880.0;
		c[6] = 321537749.0;
		c[7] = 1062287616.0;
		c[8] = 3014530821.0;

	// aprintf("C vector:\n", 9, c);

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

	// aprintf("A matrix:\n", 81, &a[0][0]);

	matmulx(n, n, a, c, d);

	// aprintf("D vector:\n", 9, d);
	// if a single element, then log d[1]
	// cov_arr_log(&d[0], 9, "result", "log.cov");
	// cov_check_arr("log.cov", "spec.cov", 9);
	// cov_spec_arr_log(&d[0], 9, threshold, "spec.cov");
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

/*
void aprintf(char* str, int n, long double a[n]) {
	printf("%s\n", str);
	int i;
	for (i = 0; i < n; i+=4) {
		printf("\t%10.15Lg", a[i]);
		if (i+1 < n) printf("\t%10.15Lg", a[i+1]);
		if (i+2 < n) printf("\t%10.15Lg", a[i+2]);
		if (i+3 < n) printf("\t%10.15Lg", a[i+3]);
		printf("\n");
	}
}
*/
