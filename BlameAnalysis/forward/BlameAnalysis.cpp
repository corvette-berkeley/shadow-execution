// Author: Cuong Nguyen

#include "BlameAnalysis.h"

/*** HELPER FUNCTIONS ***/

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
	IValue* iv = nullptr;
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

	HIGHPRECISION hResult = feval<HIGHPRECISION>(lBSO.highValue, rBSO.highValue, op);
	LOWPRECISION lResult = feval<LOWPRECISION>(lBSO.lowValue, rBSO.lowValue, op);

	BlameShadowObject* BSO = new BlameShadowObject(iid, hResult, lResult);
	executionStack.top()[inx]->setShadow(BSO);

	// Compute blame summary for the resulting shadow object.
	computeBlameSummary(*BSO, lBSO, rBSO, op);
}

/*
const BlameNode& computeBlameInformation(const BlameShadowObject& bso, const BlameShadowObject& lbso, const
BlameShadowObject& rbso, BINOP op, PRECISION p) {
  HIGHPRECISION val = clearBits(bso.highValue, DOUBLE_MANTISSA_LENGTH -PRECISION_BITS.at(PRECISION));

  PRECISION max_r = PRECISION_NO;
  for (PRECISION l = BITS_FLOAT; l < PRECISION_NO; l = PRECISION(l+1)) {
    HIGHPRECISION
    for (PRECISION r = BITS_FLOAT; r < max_j; j = PRECISION(r+1)) {

    }
  }
}
*/

void BlameAnalysis::computeBlameSummary(const BlameShadowObject& bso, const BlameShadowObject& lbso,
										const BlameShadowObject& rbso, BINOP op) {
	IID id = bso.id;

	std::vector<BlameNode> roots;
	roots.reserve(PRECISION_NO * sizeof(BlameNode));

	// Push the first blame node with precision BITS_FLOAT to roots.
	// Node with precision BITS_FLOAT does not blame anyone.
	roots.push_back(BlameNode(id, BITS_FLOAT, std::vector<BlameNode>()));

	// Compute blame information for all remained precisions.
	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO; p = PRECISION(p + 1)) {
		roots.push_back(computeBlameInformation(bso, lbso, rbso, op, p));
	}
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
