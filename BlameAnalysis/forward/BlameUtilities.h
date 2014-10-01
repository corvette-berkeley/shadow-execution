// Author: Cuong Nguyen

#ifndef _BLAME_UTILITIES_H_
#define _BLAME_UTILITIES_H_

#include <map>
#include "../../src/Common.h"

typedef double HIGHPRECISION;
typedef float LOWPRECISION;

typedef enum {
	BITS_FLOAT,
	BITS_19,
	BITS_27,
	BITS_DOUBLE,
	PRECISION_NO
} PRECISION;

const std::map<PRECISION, unsigned> PRECISION_BITS = {
	{ BITS_FLOAT, 23 }, { BITS_19, 19 }, { BITS_27, 27 }, { BITS_DOUBLE, 52 }
};

const unsigned DOUBLE_EXPONENT_LENGTH = 11;
const unsigned DOUBLE_MANTISSA_LENGTH = 52;

/*** HELPER FUNCTIONS ***/

// Replace the last "shift" bits of the double value with 0 bits.
double clearBits(double v, int shift);

// Verify whether the two double values v1 and v2 equal within the precision
// given.
//
// This function first clears the last few bits of v1 and v2 to match
// with the precision and then compares the resulting values. When comparing
// the resulting values, inequality due to rounding is torelated.
bool equalWithinPrecision(double v1, double v2, PRECISION p);

// Template function to perform the floating-point binary operator on
// the two operands and return the computed value.
template <typename T> T feval(T val01, T val02, BINOP bop);

#endif
