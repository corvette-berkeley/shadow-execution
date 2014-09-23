// Author: Cuong Nguyen

#include "BlameAnalysis.h"

/*** HELPER FUNCTIONS ***/
double BlameAnalysis::clearBits(double v, int shift) {
	int64_t mask = 0xffffffffffffffff << shift;
	int64_t* ptr = (int64_t*)&v;
	*ptr = *ptr * mask;
	double* dm = (double*)ptr;

	return *dm;
}

bool BlameAnalysis::equalWithinPrecision(double v1, double v2, PRECISION p) {
	safe_assert(p != BITS_FLOAT);

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

template <typename T> T BlameAnalysis::feval(T val01, T val02, BINOP bop) {
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

unordered_map<IID, DebugInfo> BlameAnalysis::readDebugInfo() {
	std::stringstream debugFileName;
	debugFileName << getenv("GLOG_log_dir") << separator() << "debug.bin";
	FILE* debugFile = fopen(debugFileName.str().c_str(), "rb");
	IID iid;
	struct DebugInfo debugInfo;
	unordered_map<IID, DebugInfo> debugInfoMap;

	while (fread(&iid, sizeof(uint64_t), 1, debugFile) && fread(&debugInfo, sizeof(struct DebugInfo), 1, debugFile)) {
		debugInfoMap[iid] = debugInfo;
	}
	fclose(debugFile);

	return debugInfoMap;
}

void BlameAnalysis::copyShadow(IValue* src, IValue* dest) {
	if (src->getShadow() != NULL) {
		BlameShadowObject* bsoSrc = (BlameShadowObject*)src->getShadow();
		BlameShadowObject* bsoDest = new BlameShadowObject(bsoSrc->id, bsoSrc->highValue, bsoSrc->lowValue);
		dest->setShadow(bsoDest);
	} else {
		dest->setShadow(NULL);
	}
}

const BlameShadowObject BlameAnalysis::getShadowObject(IID iid, SCOPE scope, int64_t value) {
	IValue* iv;
	switch (scope) {
		case CONSTANT: {
			double* ptr = (double*)&value;
			return BlameShadowObject(0, (HIGHPRECISION) * ptr, (LOWPRECISION) * ptr);
		}
		case GLOBAL:
			iv = globalSymbolTable[value];
			break;
		case LOCAL:
			iv = executionStack.top()[value];
			break;
		default:
			DEBUG_STDERR("Unknown scope " << scope);
			safe_assert(false);
	}

	if (iv->getShadow() == NULL) {
		double flpValue = iv->getFlpValue();
		return BlameShadowObject(iid, (HIGHPRECISION)flpValue, (LOWPRECISION)flpValue);
	} else {
		return *((BlameShadowObject*)iv->getShadow());
	}
}

void BlameAnalysis::post_fbinop(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
								KIND type UNUSED, int inx, BINOP op) {
	// Compute the resulting shadow object.
	//
	// The resulting shadow object is computed using the shadow object of the
	// left and right operand. Update the shadow object for this instruction. At
	// each time, only the most recent shadow object instant is kept.
	const BlameShadowObject lBSO = getShadowObject(liid, lScope, lValue);
	const BlameShadowObject rBSO = getShadowObject(riid, rScope, rValue);

	HIGHPRECISION hResult;
	LOWPRECISION lResult;

	switch (op) {
		case FADD:
			hResult = lBSO.highValue + rBSO.highValue;
			lResult = lBSO.lowValue + rBSO.lowValue;
			break;
		case FSUB:
			hResult = lBSO.highValue - rBSO.highValue;
			lResult = lBSO.lowValue - rBSO.lowValue;
			break;
		case FMUL:
			hResult = lBSO.highValue * rBSO.highValue;
			lResult = lBSO.lowValue * rBSO.lowValue;
			break;
		case FDIV:
			hResult = lBSO.highValue / rBSO.highValue;
			lResult = lBSO.lowValue / rBSO.lowValue;
			break;
		default:
			DEBUG_STDERR("Unsupported floating-point binary operator: " << BINOP_ToString(op));
			safe_assert(false);
	}

	BlameShadowObject* BSO = new BlameShadowObject(iid, hResult, lResult);
	executionStack.top()[inx]->setShadow(BSO);

	// Compute blame summary for the resulting shadow object.
}

/*** API FUNCTIONS ***/
void BlameAnalysis::post_fadd(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FADD);
}

void BlameAnalysis::post_fsub(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FSUB);
}

void BlameAnalysis::post_fmul(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FMUL);
}

void BlameAnalysis::post_fdiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FDIV);
}
