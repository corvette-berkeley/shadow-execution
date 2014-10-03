// Author: Cuong Nguyen

#include "BlameUtilities.h"

double clearBits(double v, int shift) {
	if (isnan(v) || v == INFINITY) {
		return v;
	}
	int64_t mask = 0xffffffffffffffff << shift;
	int64_t* ptr = (int64_t*)&v;
	*ptr = *ptr & mask;
	double* dm = (double*)ptr;

	return *dm;
}

bool equalWithinPrecision(double v1, double v2, PRECISION p) {
	if (p == BITS_DOUBLE) {
		return v1 == v2;
	}

	int64_t* ptr1 = (int64_t*)&v1;
	int64_t* ptr2 = (int64_t*)&v2;

	// Get the mantissa bits and bit-cast them to integer.
	*ptr1 =
		*ptr1 << (DOUBLE_EXPONENT_LENGTH + 1) >> (DOUBLE_EXPONENT_LENGTH + 1) >>
		(DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p) - 1);
	*ptr2 =
		*ptr2 << (DOUBLE_EXPONENT_LENGTH + 1) >> (DOUBLE_EXPONENT_LENGTH + 1) >>
		(DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p) - 1);

	// Return true if the two mantissa offset less than or equal to 1.
	return abs(*ptr1 - *ptr2) <= 1;
}
