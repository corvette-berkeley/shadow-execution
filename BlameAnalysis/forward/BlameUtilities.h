// Author: Cuong Nguyen

#ifndef _BLAME_UTILITIES_H_
#define _BLAME_UTILITIES_H_

#include <map>

typedef double HIGHPRECISION;
typedef float LOWPRECISION;

typedef enum {
	BITS_FLOAT,
	BITS_19,
	BITS_27,
	BITS_DOUBLE,
	PRECISION_NO
} PRECISION;

const map<PRECISION, unsigned> PRECISION_BITS = {{BITS_19, 19}, {BITS_27, 27}, {BITS_DOUBLE, 52}};

const unsigned DOUBLE_EXPONENT_LENGTH = 11;
const unsigned DOUBLE_MANTISSA_LENGTH = 52;

#endif
