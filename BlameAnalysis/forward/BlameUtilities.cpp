// Author: Cuong Nguyen

double clearBits(double v, int shift) {
	int64_t mask = 0xffffffffffffffff << shift;
	int64_t* ptr = (int64_t*)&v;
	*ptr = *ptr * mask;
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
	*ptr1 = *ptr1 << (DOUBLE_EXPONENT_LENGTH + 1) >> (DOUBLE_EXPONENT_LENGTH + 1) >>
			(DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p) - 1);
	*ptr2 = *ptr2 << (DOUBLE_EXPONENT_LENGTH + 1) >> (DOUBLE_EXPONENT_LENGTH + 1) >>
			(DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p) - 1);

	// Return true if the two mantissa offset less than or equal to 1.
	return abs(*ptr1 - *ptr2) <= 1;
}

template <typename T> T feval(T val01, T val02, BINOP bop) {
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
			safe_assert(false);
			DEBUG_STDERR("Unsupported floating-point binary operator: " << BINOP_ToString(bop));
	}

	return 0;
}
