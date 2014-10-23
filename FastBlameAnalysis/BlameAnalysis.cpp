#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <queue>

#include "BlameAnalysis.h"

/*** HELPER FUNCTIONS ***/

std::unordered_map<IID, DebugInfo> BlameAnalysis::readDebugInfo() {
	std::stringstream debugFileName;
	debugFileName << getenv("GLOG_log_dir") << separator() << "debug.bin";
	FILE* debugFile = fopen(debugFileName.str().c_str(), "rb");
	IID iid;
	struct DebugInfo debugInfo;
	std::unordered_map<IID, DebugInfo> debugInfoMap;

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

const BlameShadowObject BlameAnalysis::getShadowObject(IID iid,
		HIGHPRECISION v) {
	if (trace.find(iid) != trace.end()) {
		return trace[iid];
	}

	std::array<HIGHPRECISION, PRECISION_NO - 1> highValues;
	for (int i = 0; i < PRECISION_NO - 1; i++) {
		highValues[i] =
			clearBits(v, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i + 1]);
	}

	return BlameShadowObject(iid, highValues, (LOWPRECISION) v);
}

const BlameShadowObject
BlameAnalysis::shadowFEval(IID iid, const BlameShadowObject& lBSO,
						   const BlameShadowObject& rBSO, FBINOP op) {
	LOWPRECISION lowValue = feval<LOWPRECISION>(lBSO.lowValue, rBSO.lowValue, op);
	std::array<HIGHPRECISION, PRECISION_NO - 1> highValues;
	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
			p = PRECISION(p + 1)) {
		highValues[p - 1] =
			clearBits(feval<HIGHPRECISION>(lBSO.highValues[p - 1],
										   rBSO.highValues[p - 1], op),
					  DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]);
	}

	return BlameShadowObject(iid, highValues, lowValue);
}

const BlameShadowObject
BlameAnalysis::shadowFEval(IID iid, const BlameShadowObject& argBSO,
						   MATHFUNC func) {
	LOWPRECISION lowValue = mathLibEval<LOWPRECISION>(argBSO.lowValue, func);
	std::array<HIGHPRECISION, PRECISION_NO - 1> highValues;
	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
			p = PRECISION(p + 1)) {
		highValues[p - 1] =
			clearBits(mathLibEval<HIGHPRECISION>(argBSO.highValues[p - 1], func),
					  DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]);
	}

	return BlameShadowObject(iid, highValues, lowValue);
}

void BlameAnalysis::computeBlameSummary(const BlameShadowObject& BSO,
										const BlameShadowObject& lBSO,
										const BlameShadowObject& rBSO,
										FBINOP op) {
	std::array<BlameNode, PRECISION_NO> blames;
	blames[BITS_FLOAT] = BlameNode(BSO.id, BITS_FLOAT, false, false, {
		BlameNodeID(lBSO.id, BITS_FLOAT), BlameNodeID(rBSO.id, BITS_FLOAT)
	});

	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
			p = PRECISION(p + 1)) {
		blames[p] = computeBlameInformation(BSO, lBSO, rBSO, op, p);
	}

	blameSummary[BSO.id] = blames;
}

void BlameAnalysis::computeBlameSummary(const BlameShadowObject& BSO,
										const BlameShadowObject& argBSO,
										MATHFUNC func) {
	std::array<BlameNode, PRECISION_NO> blames;
	blames[BITS_FLOAT] = BlameNode(BSO.id, BITS_FLOAT, false, false, {
		BlameNodeID(argBSO.id, BITS_FLOAT)
	});

	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO;
			p = PRECISION(p + 1)) {
		blames[p] = computeBlameInformation(BSO, argBSO, func, p);
	}

	blameSummary[BSO.id] = blames;
}

BlameNode
BlameAnalysis::computeBlameInformation(const BlameShadowObject& BSO,
									   const BlameShadowObject& argBSO,
									   MATHFUNC func, PRECISION p) {
	HIGHPRECISION val = BSO.highValues[p - 1];
	bool requireHigherPrecision = val != (LOWPRECISION) val;
	bool requireHigherPrecisionOperator = true;

	// Compute the minimal blame information.
	PRECISION i = blameSummary.find(BSO.id) != blameSummary.end()
				  ? blameSummary[BSO.id][p].children[0].precision
				  : BITS_FLOAT;

	// Try all combination of i and j to find the blame that works.
	for (; i < PRECISION_NO; i = PRECISION(i + 1)) {
		if (!canBlame(val, argBSO.highValues[i - 1], func, p)) {
			continue;
		}

		break;
	}

	assert(i != PRECISION_NO && "Minimal blames cannot be found!");
	return BlameNode(BSO.id, p, requireHigherPrecision,
	requireHigherPrecisionOperator, {
		BlameNodeID(argBSO.id, i)
	});
}
BlameNode BlameAnalysis::computeBlameInformation(const BlameShadowObject& BSO,
		const BlameShadowObject& lBSO,
		const BlameShadowObject& rBSO,
		FBINOP op, PRECISION p) {
	HIGHPRECISION val = BSO.highValues[p - 1];
	bool requireHigherPrecision = val != (LOWPRECISION) val;
	bool requireHigherPrecisionOperator = true;

	// Compute the minimal blame information.
	bool found = false;
	PRECISION i, j;
	if (blameSummary.find(BSO.id) != blameSummary.end()) {
		BlameNode& bn = blameSummary[BSO.id][p];
		i = bn.children[0].precision;
		j = bn.children[1].precision;
	} else {
		i = BITS_FLOAT;
		j = BITS_FLOAT;
	}

	// Try all combination of i and j to find the blame that works.
	for (; i < PRECISION_NO; i = PRECISION(i + 1)) {
		for (; j < PRECISION_NO; j = PRECISION(j + 1)) {
			if (!canBlame(val, lBSO.highValues[i - 1], rBSO.highValues[i - 1], op,
						  p)) {
				continue;
			}

			found = true;
			requireHigherPrecisionOperator = isRequiredHigherPrecisionOperator(
												 val, lBSO.highValues[i - 1], rBSO.highValues[i - 1], op, p);
			break;
		}
		if (found) {
			break;
		}
	}

	assert(found && "Minimal blames cannot be found!");
	return BlameNode(BSO.id, p, requireHigherPrecision,
	requireHigherPrecisionOperator, {
		BlameNodeID(lBSO.id, i), BlameNodeID(rBSO.id, j)
	});
}

inline bool BlameAnalysis::canBlame(HIGHPRECISION result, HIGHPRECISION lop,
									HIGHPRECISION rop, FBINOP op, PRECISION p) {
	return equalWithinPrecision(
			   result, clearBits(feval<HIGHPRECISION>(lop, rop, op),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]),
			   p);
}

inline bool BlameAnalysis::canBlame(HIGHPRECISION result, HIGHPRECISION arg,
									MATHFUNC func, PRECISION p) {
	return equalWithinPrecision(
			   result, clearBits(mathLibEval<HIGHPRECISION>(arg, func),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]),
			   p);
}

bool BlameAnalysis::isRequiredHigherPrecisionOperator(HIGHPRECISION result,
		HIGHPRECISION lop,
		HIGHPRECISION rop,
		FBINOP op, PRECISION p) {
	return !equalWithinPrecision(
			   result, clearBits(feval<LOWPRECISION>(lop, rop, op),
								 DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]),
			   p);
}

/*** API FUNCTIONS ***/
void BlameAnalysis::fbinop(IID iid, IID liid, IID riid, HIGHPRECISION lv,
						   HIGHPRECISION rv, FBINOP op) {
	const BlameShadowObject lBSO = getShadowObject(liid, lv);
	const BlameShadowObject rBSO = getShadowObject(riid, rv);
	const BlameShadowObject BSO = shadowFEval(iid, lBSO, rBSO, op);
	trace[iid] = BSO;
	computeBlameSummary(BSO, lBSO, rBSO, op);
}

void BlameAnalysis::call_lib(IID iid, IID argiid, HIGHPRECISION argv,
							 MATHFUNC func) {
	const BlameShadowObject argBSO = getShadowObject(argiid, argv);
	const BlameShadowObject BSO = shadowFEval(iid, argBSO, func);
	trace[iid] = BSO;
	computeBlameSummary(BSO, argBSO, func);
}

void BlameAnalysis::fadd(IID iid, IID liid, IID riid, HIGHPRECISION lv,
						 HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FADD);
}

void BlameAnalysis::fsub(IID iid, IID liid, IID riid, HIGHPRECISION lv,
						 HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FSUB);
}

void BlameAnalysis::fmul(IID iid, IID liid, IID riid, HIGHPRECISION lv,
						 HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FMUL);
}

void BlameAnalysis::fdiv(IID iid, IID liid, IID riid, HIGHPRECISION lv,
						 HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FDIV);
}

void BlameAnalysis::load(IID viid, IID piid, HIGHPRECISION v) {
	trace[piid] = getShadowObject(piid, v);
	trace[viid] = trace[piid];
	if (blameSummary.find(piid) != blameSummary.end()) {
		blameSummary[viid] = blameSummary[piid];
	}
}

void BlameAnalysis::store(IID viid, IID piid, HIGHPRECISION v) {
	trace[piid] = getShadowObject(viid, v);
	if (blameSummary.find(viid) != blameSummary.end()) {
		blameSummary[piid] = blameSummary[viid];
	}
}

void BlameAnalysis::post_analysis() {
	DebugInfo dbg = debugInfoMap.at(_iid);
	std::ofstream logfile;

	logfile.open(_selfpath + ".ba");
	logfile << "Default starting point: File " << dbg.file << ", Line "
			<< dbg.line << ", Column " << dbg.column << ", IID " << _iid << "\n";
	logfile << "Default precision: " << PRECISION_BITS[_precision] << "\n";

	std::set<BlameNode> visited;
	std::queue<BlameNode> workList;
	workList.push(blameSummary[_iid][_precision]);

	while (!workList.empty()) {
		// Find more blame node and add to the queue.
		const BlameNode& node = workList.front();
		workList.pop();
		for (auto it = node.children.begin(); it != node.children.end(); it++) {
			BlameNodeID blameNodeID = *it;
			if (blameSummary.find(blameNodeID.iid) == blameSummary.end()) {
				// Children not is either a constant or alloca.
				continue;
			}
			const BlameNode& blameNode =
				blameSummary[blameNodeID.iid][blameNodeID.precision];
			if (visited.find(blameNode) == visited.end()) {
				visited.insert(blameNode);
				workList.push(blameNode);
			}
		}

		// Interpret the result for the current blame node.
		if (debugInfoMap.find(node.id.iid) == debugInfoMap.end()) {
			continue;
		}
		DebugInfo dbg = debugInfoMap.at(node.id.iid);
		if (node.requireHigherPrecision || node.requireHigherPrecisionOperator) {
			logfile << "File " << dbg.file << ", Line " << dbg.line << ", Column "
					<< dbg.column
					<< ", HigherPrecision: " << node.requireHigherPrecision
					<< ", HigherPrecisionOperator: "
					<< node.requireHigherPrecisionOperator << "\n";
		}
	}

	logfile.close();

}
