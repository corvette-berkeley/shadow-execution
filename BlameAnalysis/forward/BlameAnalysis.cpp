// Author: Cuong Nguyen

#include "BlameAnalysis.h"

/*** HELPER FUNCTIONS ***/
void BlameAnalysis::copyShadow(IValue* src, IValue* dest) {
	if (src->getShadow() != NULL) {
		BlameShadowObject* bsoSrc = (BlameShadowObject*)src->getShadow();
		BlameShadowObject* bsoDest = new BlameShadowObject(bsoSrc->id, bsoSrc->highValue, bsoSrc->lowValue);
		dest->setShadow(bsoDest);
	} else {
		dest->setShadow(NULL);
	}
}

const BlameShadowObject BlameAnalysis::getShadowObject(SCOPE scope, int64_t value) {
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
		// TODO: field IID should be obtained from ivalue.
		return BlameShadowObject(0, (HIGHPRECISION)flpValue, (LOWPRECISION)flpValue);
	} else {
		return *((BlameShadowObject*)iv->getShadow());
	}
}

/*** API FUNCTIONS ***/
/*
void BlameAnalysis::post_fbinop(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx,
BINOP op) {
  const BlameShadowObject lBSO = getShadowObject(lScope, lValue);
  const BlameShadowObject rBSO = getShadowObject(rScope, rValue);
}
*/
