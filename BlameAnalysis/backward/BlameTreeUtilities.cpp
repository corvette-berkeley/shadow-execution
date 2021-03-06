// Author: Cuong Nguyen

#include "BlameTreeUtilities.h"

bool BlameTreeUtilities::equalWithPrecision(double v1, double v2, PRECISION prec) {
	int64_t* ptr1, *ptr2;

	if (prec == BITS_DOUBLE) {
		return v1 == v2;
	}

	ptr1 = (int64_t*)&v1;
	ptr2 = (int64_t*)&v2;

	*ptr1 = *ptr1 <<12>> 12 >> (52 - exactBits(prec) - 1);
	*ptr2 = *ptr2 <<12>> 12 >> (52 - exactBits(prec) - 1);

	return abs(*ptr1 - *ptr2) <= 1;
}

double BlameTreeUtilities::clearBits(double v, int shift) {
	int64_t* ptr;
	double* dm;
	int64_t mask = 0xffffffffffffffff;
	mask = mask << shift;

	ptr = (int64_t*)&v;
	*ptr = *ptr & mask;
	dm = (double*)ptr;

	return *dm;
}

LOWPRECISION BlameTreeUtilities::feval(LOWPRECISION value01, LOWPRECISION value02, BINOP bop) {
	switch (bop) {
		case ADD:
		case FADD:
			return value01 + value02;
		case SUB:
		case FSUB:
			return value01 - value02;
		case MUL:
		case FMUL:
			return value01 * value02;
		case FDIV:
		case UDIV:
		case SDIV:
			return value01 / value02;
		case UREM:
		case SREM:
		case FREM:
		default:
			safe_assert(false);
			DEBUG_STDERR("Unsupport binary operator " << bop);
			return 0;
	}
}

HIGHPRECISION BlameTreeUtilities::eval(HIGHPRECISION value01, HIGHPRECISION value02, BINOP bop) {
	switch (bop) {
		case ADD:
		case FADD:
			return value01 + value02;
		case SUB:
		case FSUB:
			return value01 - value02;
		case MUL:
		case FMUL:
			return value01 * value02;
		case FDIV:
		case UDIV:
		case SDIV:
			return value01 / value02;
		case UREM:
		case SREM:
		case FREM:
		default:
			DEBUG_STDERR("Unsupport binary operator " << bop);
			safe_assert(false);
			return 0;
	}
}

HIGHPRECISION BlameTreeUtilities::evalFunc(HIGHPRECISION value, string func) {
	if (func.compare("sin") == 0) {
		return sin(value);
	} else if (func.compare("acos") == 0) {
		return acos(value);
	} else if (func.compare("sqrt") == 0) {
		return sqrt(value);
	} else if (func.compare("fabs") == 0) {
		return fabs(value);
	} else if (func.compare("cos") == 0) {
		return cos(value);
	} else if (func.compare("log") == 0) {
		return log(value);
	} else if (func.compare("floor") == 0) {
		return floor(value);
	} else {
		DEBUG_STDERR("Unsupport function " << func);
		safe_assert(false);
		return 0;
	}
}

int BlameTreeUtilities::exactBits(PRECISION precision) {
	switch (precision) {
		case BITS_12:
			return 12;
		case BITS_13:
			return 13;
		case BITS_14:
			return 14;
		case BITS_15:
			return 15;
		case BITS_16:
			return 16;
		case BITS_17:
			return 17;
		case BITS_18:
			return 18;
		case BITS_19:
			return 19;
		case BITS_20:
			return 20;
		case BITS_21:
			return 21;
		case BITS_22:
			return 22;
		case BITS_23:
			return 23;
		case BITS_24:
			return 24;
		case BITS_25:
			return 25;
		case BITS_26:
			return 26;
		case BITS_27:
			return 27;
		case BITS_28:
			return 28;
		case BITS_29:
			return 29;
		case BITS_30:
			return 30;
		case BITS_31:
			return 31;
		case BITS_32:
			return 32;
		case BITS_33:
			return 33;
		case BITS_DOUBLE:
			return 52;
		default:
			safe_assert(false);
			return 0;
	}
}

PRECISION BlameTreeUtilities::exactBitToPrecision(int bit) {
	switch (bit) {
		case 12:
			return BITS_12;
		case 13:
			return BITS_13;
		case 14:
			return BITS_14;
		case 15:
			return BITS_15;
		case 16:
			return BITS_16;
		case 17:
			return BITS_17;
		case 18:
			return BITS_18;
		case 19:
			return BITS_19;
		case 20:
			return BITS_20;
		case 21:
			return BITS_21;
		case 22:
			return BITS_22;
		case 23:
			return BITS_23;
		case 24:
			return BITS_24;
		case 25:
			return BITS_25;
		case 26:
			return BITS_26;
		case 27:
			return BITS_27;
		case 28:
			return BITS_28;
		case 29:
			return BITS_29;
		case 30:
			return BITS_30;
		case 31:
			return BITS_31;
		case 32:
			return BITS_32;
		case 33:
			return BITS_33;
		case 52:
			return BITS_DOUBLE;
		default:
			safe_assert(false);
			return BITS_FLOAT;
	}
}

string BlameTreeUtilities::precisionToString(PRECISION precision) {
	switch (precision) {
		case BITS_FLOAT:
			return "float";
		case BITS_12:
			return "12 bits";
		case BITS_13:
			return "13 bits";
		case BITS_14:
			return "14 bits";
		case BITS_15:
			return "15 bits";
		case BITS_16:
			return "16 bits";
		case BITS_17:
			return "17 bits";
		case BITS_18:
			return "18 bits";
		case BITS_19:
			return "19 bits";
		case BITS_20:
			return "20 bits";
		case BITS_21:
			return "21 bits";
		case BITS_22:
			return "22 bits";
		case BITS_23:
			return "23 bits";
		case BITS_24:
			return "24 bits";
		case BITS_25:
			return "25 bits";
		case BITS_26:
			return "26 bits";
		case BITS_27:
			return "27 bits";
		case BITS_28:
			return "28 bits";
		case BITS_29:
			return "29 bits";
		case BITS_30:
			return "30 bits";
		case BITS_31:
			return "31 bits";
		case BITS_32:
			return "32 bits";
		case BITS_33:
			return "33 bits";
		case BITS_DOUBLE:
			return "double";
		default:
			safe_assert(false);
			return "undefined";
	}
}
