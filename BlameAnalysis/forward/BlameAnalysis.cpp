// Author: Cuong Nguyen

#include <unistd.h>
#include <iostream>
#include <fstream>

#include "BlameAnalysis.h"
#include "../../src/InstructionMonitor.h"

using namespace std;

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

std::string BlameAnalysis::get_selfpath() {
	char buff[1024];
	ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
	if (len != -1) {
		buff[len] = '\0';
		return std::string(buff);
	} else {
		return "unknown";
	}
}

void* BlameAnalysis::copyShadow(void* oldShadow) {
	if (oldShadow != NULL) {
		BlameShadowObject* bsoSrc = (BlameShadowObject*)oldShadow;
		BlameShadowObject* bsoDest =
			new BlameShadowObject(bsoSrc->id, bsoSrc->highValue, bsoSrc->lowValue);
		return bsoDest;
	}
	return NULL;
}

const BlameShadowObject BlameAnalysis::getShadowObject(IID iid, SCOPE scope,
		int64_t value) {
	IValue* iv = nullptr;
	switch (scope) {
		case CONSTANT: {
			double* ptr = (double*)&value;
			return BlameShadowObject(iid, (HIGHPRECISION) * ptr, (LOWPRECISION) * ptr);
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

void BlameAnalysis::post_lib_call(IID iid, IID argIID, SCOPE argScope,
								  int64_t argVal, KIND type UNUSED, int inx,
								  MATHFUNC func) {
	// Compute the resulting shadow object.
	const BlameShadowObject argBSO = getShadowObject(argIID, argScope, argVal);

	HIGHPRECISION hResult = mathLibEval<HIGHPRECISION>(argBSO.highValue, func);
	LOWPRECISION lResult = mathLibEval<LOWPRECISION>(argBSO.lowValue, func);

	IValue* top = executionStack.top()[inx];
	top->setShadow(BlameShadowObject(iid, hResult, lResult));

	// Compute blame summary for the resulting shadow object.
	computeBlameSummary(*top->getShadow<BlameShadowObject>(), argBSO, func);

	// Update iid.
	_iid = iid;
}

void BlameAnalysis::computeBlameSummary(const BlameShadowObject& bso,
										const BlameShadowObject& argbso,
										MATHFUNC func) {
	IID id = bso.id;
	// Initialize the blame summary for the two operands if not exist.
	initSummaryIfNotExist(argbso.id);

	// Reuse the roots from summary or initialize a new one if not exists.
	if (blameSummary.find(id) == blameSummary.end()) {
		array<BlameNode*, PRECISION_NO> new_roots;
		new_roots[BITS_FLOAT] = new BlameNode(id, BITS_FLOAT, false, false, {
			{ nullptr }
		});
		safe_assert(new_roots[BITS_FLOAT]->getSize() == 0);
		for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
				p = PRECISION(p + 1)) {
			new_roots[p] = new BlameNode(id, p, false, false, {
				{ blameSummary[argbso.id][BITS_FLOAT], nullptr }
			});
			safe_assert(new_roots[p]->getSize() == 1);
		}
		blameSummary[id] = std::move(new_roots);
	}
	const std::array<BlameNode*, PRECISION_NO>& roots = blameSummary[id];

	// Compute blame information for all remained precisions and merge with the
	// current blame summary.
	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
			p = PRECISION(p + 1)) {
		const BlameNode& blameInfo = computeBlameInformation(bso, argbso, func, p);
		mergeBlame(roots[p], blameInfo);
	}
}

const BlameNode
BlameAnalysis::computeBlameInformation(const BlameShadowObject& bso,
									   const BlameShadowObject& argbso,
									   MATHFUNC func, PRECISION p) {
	HIGHPRECISION val =
		clearBits(bso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]);
	bool requireHigherPrecision = val != (LOWPRECISION) val;
	bool requireHigherPrecisionOperator = true;

	// Compute the values of lbso and rbso in different precision.
	std::array<HIGHPRECISION, PRECISION_NO> argbsoVals;
	argbsoVals[BITS_FLOAT] = argbso.lowValue;
	for (PRECISION i = PRECISION(BITS_FLOAT + 1); i < PRECISION_NO;
			i = PRECISION(i + 1)) {
		argbsoVals[i] =
			clearBits(argbso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
	}

	// Compute the minimal blame information.
	bool found = false;
	PRECISION i;
	for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
		if (!canBlame(val, argbsoVals[i], func, p)) {
			continue;
		}

		// Found a blame pair. Terminate the loop.
		found = true;
		break;
	}

	// Construct the associated blame node and return.
	safe_assert(found);
	BlameNode* argBlameNode = blameSummary[argbso.id][i];
	safe_assert(argBlameNode != NULL);

	auto bn = BlameNode(bso.id, p, requireHigherPrecision,
	requireHigherPrecisionOperator, {
		{ argBlameNode, nullptr }
	});
	safe_assert(bn.getSize() == 1);
	return bn;
}

inline bool BlameAnalysis::canBlame(HIGHPRECISION result, HIGHPRECISION arg,
									MATHFUNC func, PRECISION p) {
	return equalWithinPrecision(
			   result, clearBits(mathLibEval<HIGHPRECISION>(arg, func),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]),
			   p);
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

	IValue* top = executionStack.top()[inx];

	top->setShadow<BlameShadowObject>(BlameShadowObject(iid, hResult, lResult));
	BlameShadowObject* BSO = top->getShadow<BlameShadowObject>();

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

	// Initialize the blame summary for the two operands if not exist. They can be
	// non-exists if their values are constant.
	initSummaryIfNotExist(lbso.id);
	initSummaryIfNotExist(rbso.id);

	// Reuse the roots from summary or initialize a new one if not exists.
	if (blameSummary.find(id) == blameSummary.end()) {
		array<BlameNode*, PRECISION_NO> new_roots;
		new_roots[BITS_FLOAT] = new BlameNode(id, BITS_FLOAT, false, false, {
			{ nullptr }
		});
		safe_assert(new_roots[BITS_FLOAT]->getSize() == 0);
		for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
				p = PRECISION(p + 1)) {
			new_roots[p] = new BlameNode(id, p, false, false, {
				{ blameSummary[lbso.id][BITS_FLOAT], blameSummary[rbso.id][BITS_FLOAT] }
			});
			safe_assert(new_roots[p]->getSize() == 2);
		}
		blameSummary[id] = std::move(new_roots);
	}
	const std::array<BlameNode*, PRECISION_NO>& roots = blameSummary[id];

	// Compute blame information for all remained precisions and merge with the
	// current blame summary.
	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
			p = PRECISION(p + 1)) {
		constructBlameSummary(bso, lbso, rbso, op, p, roots[p]);
		//		const BlameNode& blameInfo = computeBlameInformation(bso, lbso, rbso,
		//op, p);
		//		mergeBlame(roots[p], blameInfo);
	}
}

void BlameAnalysis::constructBlameSummary(const BlameShadowObject& bso,
		const BlameShadowObject& lbso,
		const BlameShadowObject& rbso,
		BINOP op, PRECISION p,
		BlameNode* bn) {
	HIGHPRECISION val =
		clearBits(bso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]);
	bool requireHigherPrecision = val != (LOWPRECISION) val;
	bool requireHigherPrecisionOperator = true;

	// Compute the values of lbso and rbso in different precision.
	std::array<HIGHPRECISION, PRECISION_NO> lbsoVals;
	std::array<HIGHPRECISION, PRECISION_NO> rbsoVals;
	lbsoVals[BITS_FLOAT] = lbso.lowValue;
	rbsoVals[BITS_FLOAT] = rbso.lowValue;
	for (PRECISION i = PRECISION(BITS_FLOAT + 1); i < PRECISION_NO;
			i = PRECISION(i + 1)) {
		lbsoVals[i] =
			clearBits(lbso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
		rbsoVals[i] =
			clearBits(rbso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
	}

	// Compute the minimal blame information.
	PRECISION i, j;

	bool found = false;
	for (i = bn->children[0]->precision; i < PRECISION_NO; i = PRECISION(i + 1)) {
		for (j = bn->children[0]->precision; j < PRECISION_NO;
				j = PRECISION(j + 1)) {
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

	safe_assert(found);
	bn->children[0] = blameSummary[lbso.id][i];
	bn->children[1] = blameSummary[rbso.id][j];
	bn->requireHigherPrecision =
		requireHigherPrecision || bn->requireHigherPrecision;
	bn->requireHigherPrecisionOperator =
		requireHigherPrecisionOperator || bn->requireHigherPrecisionOperator;
}

const BlameNode BlameAnalysis::computeBlameInformation(
	const BlameShadowObject& bso, const BlameShadowObject& lbso,
	const BlameShadowObject& rbso, BINOP op, PRECISION p) {
	HIGHPRECISION val =
		clearBits(bso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]);
	bool requireHigherPrecision = val != (LOWPRECISION) val;
	bool requireHigherPrecisionOperator = true;

	// Compute the values of lbso and rbso in different precision.
	std::array<HIGHPRECISION, PRECISION_NO> lbsoVals;
	std::array<HIGHPRECISION, PRECISION_NO> rbsoVals;
	lbsoVals[BITS_FLOAT] = lbso.lowValue;
	rbsoVals[BITS_FLOAT] = rbso.lowValue;
	for (PRECISION i = PRECISION(BITS_FLOAT + 1); i < PRECISION_NO;
			i = PRECISION(i + 1)) {
		lbsoVals[i] =
			clearBits(lbso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
		rbsoVals[i] =
			clearBits(rbso.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
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
	safe_assert(found);
	BlameNode* lBlameNode = blameSummary[lbso.id][i];
	BlameNode* rBlameNode = blameSummary[rbso.id][j];
	safe_assert(lBlameNode != NULL);
	safe_assert(rBlameNode != NULL);

	auto bn = BlameNode(bso.id, p, requireHigherPrecision,
	requireHigherPrecisionOperator, {
		{ lBlameNode, rBlameNode }
	});
	safe_assert(bn.getSize() == 2);
	return bn;
}

inline bool BlameAnalysis::canBlame(HIGHPRECISION result, HIGHPRECISION lop,
									HIGHPRECISION rop, BINOP op, PRECISION p) {
	return equalWithinPrecision(
			   result, clearBits(feval<HIGHPRECISION>(lop, rop, op),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]),
			   p);
}

bool BlameAnalysis::isRequiredHigherPrecisionOperator(HIGHPRECISION result,
		HIGHPRECISION lop,
		HIGHPRECISION rop,
		BINOP op, PRECISION p) {
	return !equalWithinPrecision(
			   result, clearBits(feval<LOWPRECISION>(lop, rop, op),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]),
			   p);
}

void BlameAnalysis::mergeBlame(BlameNode* summary, const BlameNode& blame) {
	// Summary and blame node needs to have same precision requirement and same
	// numbers of blame children.
	safe_assert(summary->precision == blame.precision &&
				summary->getSize() == blame.getSize());
	std::array<BlameNode*, 2> merge = { { nullptr, nullptr } };
	int i = 0;
	for (auto sIt = summary->cbegin(), bIt = blame.cbegin();
			sIt != summary->cend(); sIt++, bIt++) {
		BlameNode* sNode = *sIt;
		BlameNode* bNode = *bIt;
		safe_assert(sNode != NULL);
		safe_assert(bNode != NULL);
		if (sNode->precision > bNode->precision) {
			merge[i] = sNode;
		} else {
			merge[i] = bNode;
		}
		i++;
	}
	summary->setChildren(merge);
	summary->requireHigherPrecision =
		summary->requireHigherPrecision || blame.requireHigherPrecision;
	summary->requireHigherPrecisionOperator =
		summary->requireHigherPrecisionOperator ||
		blame.requireHigherPrecisionOperator;
}

void BlameAnalysis::initSummaryIfNotExist(IID id) {
	if (blameSummary.find(id) == blameSummary.end()) {
		std::array<BlameNode*, PRECISION_NO> blames;
		for (PRECISION p = BITS_FLOAT; p < PRECISION_NO; p = PRECISION(p + 1)) {
			blames[p] = new BlameNode(id, p, false, false, {
				{ nullptr }
			});
			safe_assert(blames[p]->getSize() == 0);
		}
		blameSummary[id] = blames;
	}
}

/*** API FUNCTIONS ***/
void BlameAnalysis::pre_analysis() {
	IValue::setShadowHandlers(copyShadow, [](void* a) {
		delete static_cast<BlameShadowObject*>(a);
	});
}

void BlameAnalysis::post_call_sin(IID iid, IID argIID, SCOPE argScope,
								  int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, SIN);
}

void BlameAnalysis::post_call_exp(IID iid, IID argIID, SCOPE argScope,
								  int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, EXP);
}

void BlameAnalysis::post_call_cos(IID iid, IID argIID, SCOPE argScope,
								  int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, COS);
}

void BlameAnalysis::post_call_acos(IID iid, IID argIID, SCOPE argScope,
								   int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, ACOS);
}

void BlameAnalysis::post_call_sqrt(IID iid, IID argIID, SCOPE argScope,
								   int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, SQRT);
}

void BlameAnalysis::post_call_fabs(IID iid, IID argIID, SCOPE argScope,
								   int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, FABS);
}

void BlameAnalysis::post_call_log(IID iid, IID argIID, SCOPE argScope,
								  int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, LOG);
}

void BlameAnalysis::post_call_floor(IID iid, IID argIID, SCOPE argScope,
									int64_t argVal, KIND type, int inx) {
	post_lib_call(iid, argIID, argScope, argVal, type, inx, FLOOR);
}

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
	ofstream logfile;

	logfile.open(_selfpath + ".ba");
	logfile << "Default starting point: File " << dbg.file << ", Line "
			<< dbg.line << ", Column " << dbg.column << ", IID " << _iid << "\n";
	logfile << "Default precision: " << PRECISION_BITS[_precision] << "\n";

	std::set<BlameNode*> visited;
	std::queue<BlameNode*> workList;
	workList.push(blameSummary[_iid][_precision]);

	while (!workList.empty()) {
		// Find more blame node and add to the queue.
		BlameNode* node = workList.front();
		workList.pop();
		for (auto it = node->cbegin(); it != node->cend(); it++) {
			BlameNode* blameNode = *it;
			if (visited.find(blameNode) == visited.end()) {
				visited.insert(blameNode);
				workList.push(blameNode);
			}
		}

		// Interpret the result for the current blame node.
		if (debugInfoMap.find(node->iid) == debugInfoMap.end()) {
			continue;
		}
		DebugInfo dbg = debugInfoMap.at(node->iid);
		if (node->requireHigherPrecision || node->requireHigherPrecisionOperator) {
			logfile << "File " << dbg.file << ", Line " << dbg.line << ", Column "
					<< dbg.column
					<< ", HigherPrecision: " << node->requireHigherPrecision
					<< ", HigherPrecisionOperator: "
					<< node->requireHigherPrecisionOperator << "\n";
		}
	}

	logfile.close();

	// Free memory
	for (auto it = blameSummary.begin(); it != blameSummary.end(); it++) {
		array<BlameNode*, PRECISION_NO>& nodes = it->second;
		for (auto it2 = nodes.begin(); it2 != nodes.end(); it2++) {
			delete *it2;
		}
	}
	blameSummary.clear();
}

static RegisterObserver<BlameAnalysis> BlameAnalysisInstance("blameanalysis");
