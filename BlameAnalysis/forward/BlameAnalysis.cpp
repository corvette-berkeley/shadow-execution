// Author: Cuong Nguyen

#include "BlameAnalysis.h"
#include "../../src/InstructionMonitor.h"

/*** HELPER FUNCTIONS ***/

unordered_map<IID, DebugInfo> BlameAnalysis::readDebugInfo() {
	std::stringstream debugFileName;
	debugFileName << getenv("GLOG_log_dir") << separator() << "debug.bin";
	FILE* debugFile = fopen(debugFileName.str().c_str(), "rb");
	IID iid;
	struct DebugInfo debugInfo;
	unordered_map<IID, DebugInfo> debugInfoMap;

	while (fread(&iid, sizeof(uint64_t), 1, debugFile) &&
			fread(&debugInfo, sizeof(struct DebugInfo), 1, debugFile)) {
		debugInfoMap[iid] = debugInfo;
	}
	fclose(debugFile);

	return debugInfoMap;
}

void BlameAnalysis::copyShadow(IValue* src, IValue* dest) {
	if (src->getShadow() != NULL) {
		BlameShadowObject* bsoSrc = (BlameShadowObject*)src->getShadow();
		BlameShadowObject* bsoDest =
			new BlameShadowObject(bsoSrc->id, bsoSrc->highValue, bsoSrc->lowValue);
		dest->setShadow(bsoDest);
	} else {
		dest->setShadow(NULL);
	}
}

const BlameShadowObject BlameAnalysis::getShadowObject(IID iid, SCOPE scope,
		int64_t value) {
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
		return BlameShadowObject(iid, (HIGHPRECISION) flpValue,
								 (LOWPRECISION) flpValue);
	} else {
		return *((BlameShadowObject*)iv->getShadow());
	}
}

void BlameAnalysis::post_fbinop(IID iid, IID liid, IID riid, SCOPE lScope,
								SCOPE rScope, int64_t lValue, int64_t rValue,
								KIND type UNUSED, int inx, BINOP op) {
	// Compute the resulting shadow object.
	//
	// The resulting shadow object is computed using the shadow object of the
	// left and right operand. Update the shadow object for this instruction. At
	// each time, only the most recent shadow object instant is kept.
	const BlameShadowObject lBSO = getShadowObject(liid, lScope, lValue);
	const BlameShadowObject rBSO = getShadowObject(riid, rScope, rValue);

	HIGHPRECISION hResult =
		feval<HIGHPRECISION>(lBSO.highValue, rBSO.highValue, op);
	LOWPRECISION lResult = feval<LOWPRECISION>(lBSO.lowValue, rBSO.lowValue, op);

	BlameShadowObject* BSO = new BlameShadowObject(iid, hResult, lResult);
	executionStack.top()[inx]->setShadow(BSO);

	// Compute blame summary for the resulting shadow object.
	computeBlameSummary(*BSO, lBSO, rBSO, op);

	// Update iid.
	_iid = iid;
}

void BlameAnalysis::computeBlameSummary(const BlameShadowObject& bso,
										const BlameShadowObject& lbso,
										const BlameShadowObject& rbso,
										BINOP op) {
	IID id = bso.id;

	std::vector<BlameNode> roots;
	roots.reserve(PRECISION_NO * sizeof(BlameNode));

	// Push the first blame node with precision BITS_FLOAT to roots.
	// Node with precision BITS_FLOAT does not blame anyone.
	roots.push_back(
		BlameNode(id, BITS_FLOAT, false, false, std::vector<BlameNode>()));

	// Compute blame information for all remained precisions.
	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
			p = PRECISION(p + 1)) {
		roots.push_back(computeBlameInformation(bso, lbso, rbso, op, p));
	}

	// Merge summary if previous blame information exists.
	if (blameSummary.find(id) != blameSummary.end()) {
		blameSummary[id] = roots;
	} else {
		blameSummary[id] = mergeBlame(blameSummary[id], roots);
	}
}

const BlameNode BlameAnalysis::computeBlameInformation(
	const BlameShadowObject& bso, const BlameShadowObject& lbso,
	const BlameShadowObject& rbso, BINOP op, PRECISION p) {
	HIGHPRECISION val =
		clearBits(bso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p));
	bool requireHigherPrecision = val != (LOWPRECISION) val;
	bool requireHigherPrecisionOperator = true;

	// Compute the values of lbso and rbso in different precision.
	std::map<PRECISION, HIGHPRECISION> lbsoVals;
	std::map<PRECISION, HIGHPRECISION> rbsoVals;
	lbsoVals[BITS_FLOAT] = lbso.lowValue;
	rbsoVals[BITS_FLOAT] = rbso.lowValue;
	for (PRECISION i = PRECISION(BITS_FLOAT + 1); i < PRECISION_NO;
			i = PRECISION(i + 1)) {
		lbsoVals[i] = clearBits(lbso.highValue,
								DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(i));
		rbsoVals[i] = clearBits(rbso.highValue,
								DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(i));
	}

	// Compute the minimal blame information.
	PRECISION i, j;
	bool found = false;
	for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
		for (j = BITS_FLOAT; j < PRECISION_NO; j = PRECISION(j + 1)) {
			if (!canBlame(val, lbsoVals[i], rbsoVals[j], op, p)) {
				continue;
			}

			// Found a blame pair. Terminate the loop.
			found = true;
			requireHigherPrecisionOperator = isRequiredHigherPrecisionOperator(
												 val, lbsoVals[i], rbsoVals[j], op, p);
			break;
		}
		// Terminate the loop if found.
		if (found) {
			break;
		}
	}

	// Construct the associated blame node and return.
	BlameNode& lBlameNode = blameSummary[lbso.id][i];
	BlameNode& rBlameNode = blameSummary[rbso.id][j];
	std::vector<BlameNode> blameNodes = { lBlameNode, rBlameNode };

	return BlameNode(bso.id, p, requireHigherPrecision,
					 requireHigherPrecisionOperator, blameNodes);
}

bool BlameAnalysis::canBlame(HIGHPRECISION result, HIGHPRECISION lop,
							 HIGHPRECISION rop, BINOP op, PRECISION p) {
	return equalWithinPrecision(
			   result, clearBits(feval<HIGHPRECISION>(lop, rop, op),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p)),
			   p);
}

bool BlameAnalysis::isRequiredHigherPrecisionOperator(HIGHPRECISION result,
		HIGHPRECISION lop,
		HIGHPRECISION rop,
		BINOP op, PRECISION p) {
	return !equalWithinPrecision(
			   result, clearBits(feval<LOWPRECISION>(lop, rop, op),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS.at(p)),
			   p);
}

std::vector<BlameNode> BlameAnalysis::mergeBlame(std::vector<BlameNode> summary,
		std::vector<BlameNode> blame) {
	std::vector<BlameNode>::iterator sIt, bIt;
	for (sIt = summary.begin(), bIt = blame.begin(); sIt != summary.end();
			sIt++, bIt++) {
		BlameNode& sNode = *sIt;
		BlameNode& bNode = *bIt;
		mergeBlame(sNode, bNode);
	}

	return summary;
}

void BlameAnalysis::mergeBlame(BlameNode& summary, BlameNode& blame) {
	// Summary and blame node needs to have same precision requirement and same
	// numbers of blame children.
	safe_assert(summary.precision == blame.precision &&
				summary.children.size() == blame.children.size());
	std::vector<BlameNode> merge;
	std::vector<BlameNode>::iterator sIt, bIt;
	for (sIt = summary.children.begin(), bIt = blame.children.begin();
			sIt != summary.children.end(); sIt++, bIt++) {
		BlameNode& sNode = *sIt;
		BlameNode& bNode = *bIt;
		if (sNode.precision > bNode.precision) {
			merge.push_back(sNode);
		} else {
			merge.push_back(bNode);
		}
	}
	summary.children = merge;
}

/*** API FUNCTIONS ***/
void BlameAnalysis::post_fadd(IID iid, IID liid, IID riid, SCOPE lScope,
							  SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FADD);
}

void BlameAnalysis::post_fsub(IID iid, IID liid, IID riid, SCOPE lScope,
							  SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FSUB);
}

void BlameAnalysis::post_fmul(IID iid, IID liid, IID riid, SCOPE lScope,
							  SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FMUL);
}

void BlameAnalysis::post_fdiv(IID iid, IID liid, IID riid, SCOPE lScope,
							  SCOPE rScope, int64_t lValue, int64_t rValue,
							  KIND type, int inx) {
	post_fbinop(iid, liid, riid, lScope, rScope, lValue, rValue, type, inx, FDIV);
}

// Interpretation of result.
void BlameAnalysis::post_analysis() {
	DebugInfo dbg = debugInfoMap.at(_iid);
	printf("Default starting point: Function %s, Line %d, Column %d\n", dbg.file,
		   dbg.line, dbg.column);
	printf("Default precision: %d\n", PRECISION_BITS.at(_precision));

	std::set<BlameNode> visited;
	std::queue<BlameNode> workList;
	workList.push(blameSummary[_iid][_precision]);

	while (!workList.empty()) {
		// Find more blame node and add to the queue.
		BlameNode& node = workList.front();
		workList.pop();
		std::vector<BlameNode> blameNodes = node.children;
		for (std::vector<BlameNode>::iterator it = blameNodes.begin();
				it != blameNodes.end(); it++) {
			BlameNode& blameNode = *it;
			if (visited.find(blameNode) == visited.end()) {
				visited.insert(blameSummary[blameNode.iid][blameNode.precision]);
				workList.push(blameSummary[blameNode.iid][blameNode.precision]);
			}
		}

		// Interpret the result for the current blame node.
		DebugInfo dbg = debugInfoMap.at(node.iid);
		if (node.requireHigherPrecision || node.requireHigherPrecisionOperator) {
			printf("Function %s, Line %d, Column %d, HigherPrecision: %d, "
				   "HigherPrecisionOperator: %d\n",
				   dbg.file, dbg.line, dbg.column, node.requireHigherPrecision,
				   node.requireHigherPrecisionOperator);
		}
	}
}

static RegisterObserver<BlameAnalysis> BlameAnalysisInstance("blameanalysis");
