#ifndef _BLAME_UTILITIES_H_
#define _BLAME_UTILITIES_H_

#include <assert.h>
#include <cmath>
#include <unordered_map>
#include <istream>

struct DebugInfo {
	std::string file;
	unsigned line, column;
	DebugInfo() : file("n/a"), line(0), column(0) {}
};

std::istream& operator>>(std::istream& input, DebugInfo& dbg);

typedef int32_t IID;
typedef double HIGHPRECISION;
typedef float LOWPRECISION;

typedef enum {
	BITS_FLOAT,
	BITS_13,
	BITS_19,
	BITS_27,
	BITS_33,
	BITS_DOUBLE,
	PRECISION_NO
} PRECISION;

typedef enum {
	FADD,
	FSUB,
	FMUL,
	FDIV,
	FBINOP_NO
} FBINOP;

typedef enum {
	OEQ,
	OGT,
	OGE,
	OLT,
	OLE,
	ONE,
	CMPOP_NO
} CMPOP;

typedef enum {
	SIN,
	ACOS,
	SQRT,
	FABS,
	COS,
	LOG,
	EXP,
	FLOOR,
	MATHFUNC_NO
} MATHFUNC;

const std::array<unsigned, PRECISION_NO> PRECISION_BITS = {{
		23,  // BITS_FLOAT
		13,  // BITS_13
		19,  // BITS_19
		27,  // BITS_27
		33,  // BITS_33
		52  // BITS_DOUBLE
	}
};

const unsigned DOUBLE_EXPONENT_LENGTH = 11;
const unsigned DOUBLE_MANTISSA_LENGTH = 52;

/*** HELPER FUNCTIONS ***/

// Replace the last "shift" bits of the double value with 0 bits.
inline double clearBits(double v, int shift) {
	if (std::isnan(v) || std::isinf(v)) {
		return v;
	}

	int64_t mask = 0xffffffffffffffff << shift;
	int64_t* ptr = (int64_t*)&v;
	*ptr = *ptr & mask;
	double* dm = (double*)ptr;

	return *dm;
}

// Verify whether the two double values v1 and v2 equal within the precision
// given.
//
// This function first clears the last few bits of v1 and v2 to match
// with the precision and then compares the resulting values. When comparing
// the resulting values, inequality due to rounding is torelated.
inline bool equalWithinPrecision(double v1, double v2, PRECISION p) {
	if (std::isnan(v1)) {
		return std::isnan(v2);
	}
	if (std::isinf(v1)) {
		return std::isinf(v2);
	}
	if (p == BITS_DOUBLE) {
		return v1 == v2;
	}

	int64_t* ptr1 = (int64_t*)&v1;
	int64_t* ptr2 = (int64_t*)&v2;

	// Get the mantissa bits and bit-cast them to integer.
	*ptr1 = *ptr1 << (DOUBLE_EXPONENT_LENGTH + 1) >> (DOUBLE_EXPONENT_LENGTH + 1) >>
			(DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p) - 1);
	*ptr2 = *ptr2 << (DOUBLE_EXPONENT_LENGTH + 1) >> (DOUBLE_EXPONENT_LENGTH + 1) >>
			(DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p) - 1);

	// Return true if the two mantissa offset less than or equal to 1.
	return abs(*ptr1 - *ptr2) <= 1;
}

// Template function to perform the floating-point binary operator on
// the two operands and return the computed value.
template <typename T> T feval(T val01, T val02, FBINOP bop) {
	switch (bop) {
		case FADD:
			return val01 + val02;
		case FSUB:
			return val01 - val02;
		case FMUL:
			return val01 * val02;
		case FDIV:
			return val01 / val02;
		default:
			assert(false && "Unsuppored floating-point binary operator.");
	}

	return 0;
}

template <typename T> bool fcmp_eval(T val01, T val02, CMPOP op) {
	switch (op) {
		case OEQ:
			return val01 == val02;
		case OGT:
			return val01 > val02;
		case OGE:
			return val01 >= val02;
		case OLT:
			return val01 < val02;
		case OLE:
			return val01 <= val02;
		case ONE:
			return val01 != val02;
		default:
			assert(false && "Unsupported floating-point comparision operator.");
	}

	return false;
}

template <typename T> T mathLibEval(T val, MATHFUNC func) {
	switch (func) {
		case SIN:
			return sin(val);
		case ACOS:
			return acos(val);
		case SQRT:
			return sqrt(val);
		case FABS:
			return fabs(val);
		case COS:
			return cos(val);
		case LOG:
			return log(val);
		case FLOOR:
			return floor(val);
		case EXP:
			return exp(val);
		default:
			assert(false && "Unsupported math library call.");
	}

	return 0;
}

#endif
