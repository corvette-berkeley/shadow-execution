#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <set>
#include <queue>

#include "BlameAnalysis.h"
using namespace std;

/*** HELPER FUNCTIONS ***/

std::unordered_map<IID, DebugInfo> BlameAnalysis::readDebugInfo() {
	ifstream fin("debug.bin");
	std::unordered_map<IID, DebugInfo> debugInfoMap;
	while (fin) {
		IID id;
		DebugInfo dbg;
		string a;
		fin >> a >> id >> dbg;
		debugInfoMap[id] = dbg;
	}

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

const BlameShadowObject BlameAnalysis::getShadowObject(IID iid, HIGHPRECISION v) {
	if (trace.find(iid) == trace.end()) {
		//		cout << "IID: " << iid << " is a constant." << endl;
		return BlameShadowObject(iid, (LOWPRECISION)v, v);
	}

	if (trace[iid][0].highValue != v) {
		cout << "Get Shadow" << endl;
		cout << iid << endl;
		cout << setprecision(10) << trace[iid][0].highValue << endl;
		cout << setprecision(10) << v << endl;
		cout << "----" << endl;
		exit(5);
	}

	return trace[iid][0];
}

const BlameShadowObject BlameAnalysis::shadowFEval(IID iid, const BlameShadowObject& lBSO,
		const BlameShadowObject& rBSO, FBINOP op) {
	LOWPRECISION lowValue = feval<LOWPRECISION>(lBSO.lowValue, rBSO.lowValue, op);
	HIGHPRECISION highValue = feval<HIGHPRECISION>(lBSO.highValue, rBSO.highValue, op);

	return BlameShadowObject(iid, lowValue, highValue);
}

const BlameShadowObject BlameAnalysis::shadowFEval(IID iid, const BlameShadowObject& argBSO, MATHFUNC func) {
	LOWPRECISION lowValue = mathLibEval<LOWPRECISION>(argBSO.lowValue, func);
	HIGHPRECISION highValue = mathLibEval<HIGHPRECISION>(argBSO.highValue, func);

	return BlameShadowObject(iid, lowValue, highValue);
}

void BlameAnalysis::computeBlameSummary(const BlameShadowObject& BSO, const BlameShadowObject& lBSO,
										const BlameShadowObject& rBSO, FBINOP op) {
	std::array<BlameNode, PRECISION_NO> blames;
	blames[BITS_FLOAT] =
		BlameNode(BSO.id, BITS_FLOAT, false, false, {BlameNodeID(lBSO.id, BITS_FLOAT), BlameNodeID(rBSO.id, BITS_FLOAT)});

	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO; p = PRECISION(p + 1)) {
		blames[p] = computeBlameInformation(BSO, lBSO, rBSO, op, p);
	}

	blameSummary[BSO.id] = blames;
}

void BlameAnalysis::computeBlameSummary(const BlameShadowObject& BSO, const BlameShadowObject& argBSO, MATHFUNC func) {
	std::array<BlameNode, PRECISION_NO> blames;
	blames[BITS_FLOAT] = BlameNode(BSO.id, BITS_FLOAT, false, false, {BlameNodeID(argBSO.id, BITS_FLOAT)});

	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO; p = PRECISION(p + 1)) {
		blames[p] = computeBlameInformation(BSO, argBSO, func, p);
	}

	blameSummary[BSO.id] = blames;
}

BlameNode BlameAnalysis::computeBlameInformation(const BlameShadowObject& BSO, const BlameShadowObject& argBSO,
		MATHFUNC func, PRECISION p) {
	HIGHPRECISION val = BSO.highValue;
	bool requireHigherPrecision = val != (LOWPRECISION)val;
	bool requireHigherPrecisionOperator = true;

	// Compute the values of argbso in different precision.
	std::array<HIGHPRECISION, PRECISION_NO> argbsoVals;
	argbsoVals[BITS_FLOAT] = argBSO.lowValue;
	for (PRECISION i = PRECISION(BITS_FLOAT + 1); i < PRECISION_NO; i = PRECISION(i + 1)) {
		argbsoVals[i] = clearBits(argBSO.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
	}

	// Compute the minimal blame information.
	PRECISION min_i =
		blameSummary.find(BSO.id) != blameSummary.end() ? blameSummary[BSO.id][p].children[0].precision : BITS_FLOAT;

	// Try all i to find the blame that works.
	PRECISION i;
	for (i = min_i; i < PRECISION_NO; i = PRECISION(i + 1)) {
		if (!canBlame(val, argbsoVals[i], func, p)) {
			continue;
		}

		break;
	}

	assert(i != PRECISION_NO && "Minimal blames cannot be found!");
	return BlameNode(BSO.id, p, requireHigherPrecision, requireHigherPrecisionOperator, {BlameNodeID(argBSO.id, i)});
}
BlameNode BlameAnalysis::computeBlameInformation(const BlameShadowObject& BSO, const BlameShadowObject& lBSO,
		const BlameShadowObject& rBSO, FBINOP op, PRECISION p) {
	HIGHPRECISION val = BSO.highValue;
	bool requireHigherPrecision = val != (LOWPRECISION)val;
	bool requireHigherPrecisionOperator = true;

	// Compute values for lbso and rbso in different precision.
	std::array<HIGHPRECISION, PRECISION_NO> lbsoVals;
	std::array<HIGHPRECISION, PRECISION_NO> rbsoVals;
	lbsoVals[BITS_FLOAT] = lBSO.lowValue;
	rbsoVals[BITS_FLOAT] = rBSO.lowValue;
	for (PRECISION i = PRECISION(BITS_FLOAT + 1); i < PRECISION_NO; i = PRECISION(i + 1)) {
		lbsoVals[i] = clearBits(lBSO.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
		rbsoVals[i] = clearBits(rBSO.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]);
	}

	// Compute the minimal blame information.
	bool found = false;
	PRECISION min_i = BITS_FLOAT;
	PRECISION min_j = BITS_FLOAT;
	if (blameSummary.find(BSO.id) != blameSummary.end()) {
		BlameNode& bn = blameSummary[BSO.id][p];
		min_i = bn.children[0].precision;
		min_j = bn.children[1].precision;
	}

	PRECISION i = min_i;
	PRECISION j = min_j;
	// Try all combination of i and j to find the blame that works.
	for (i = min_i; i < PRECISION_NO; i = PRECISION(i + 1)) {
		for (j = min_j; j < PRECISION_NO; j = PRECISION(j + 1)) {
			if (!canBlame(val, lbsoVals[i], rbsoVals[j], op, p)) {
				continue;
			}

			found = true;
			requireHigherPrecisionOperator = isRequiredHigherPrecisionOperator(val, lbsoVals[i], rbsoVals[j], op, p);
			break;
		}
		if (found) {
			break;
		}
	}

	assert(found && "Minimal blames cannot be found!");
	return BlameNode(BSO.id, p, requireHigherPrecision, requireHigherPrecisionOperator,
	{BlameNodeID(lBSO.id, i), BlameNodeID(rBSO.id, j)});
}

inline bool BlameAnalysis::canBlame(HIGHPRECISION result, HIGHPRECISION lop, HIGHPRECISION rop, FBINOP op,
									PRECISION p) {
	return equalWithinPrecision(
			   result, clearBits(feval<HIGHPRECISION>(lop, rop, op), DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]), p);
}

inline bool BlameAnalysis::canBlame(HIGHPRECISION result, HIGHPRECISION arg, MATHFUNC func, PRECISION p) {
	return equalWithinPrecision(
			   result, clearBits(mathLibEval<HIGHPRECISION>(arg, func), DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]), p);
}

bool BlameAnalysis::isRequiredHigherPrecisionOperator(HIGHPRECISION result, HIGHPRECISION lop, HIGHPRECISION rop,
		FBINOP op, PRECISION p) {
	return !equalWithinPrecision(
			   result, clearBits(feval<LOWPRECISION>(lop, rop, op), DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]), p);
}

void BlameAnalysis::copyBlameSummary(IID dest, IID src) {
	if (blameSummary.find(src) != blameSummary.end()) {
		blameSummary[dest] = blameSummary[src];
	}
}

/*** API FUNCTIONS ***/
void BlameAnalysis::fbinop(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv, FBINOP op) {
	const BlameShadowObject lBSO = getShadowObject(liid, lv);
	const BlameShadowObject rBSO = getShadowObject(riid, rv);
	const BlameShadowObject BSO = shadowFEval(iid, lBSO, rBSO, op);

	if (BSO.highValue != feval<HIGHPRECISION>(lv, rv, op)) {
		cout << "IID: " << iid << endl;
		cout << "RIID: " << riid << endl;
		cout << "LIID: " << liid << endl;
		cout << setprecision(10) << lv << endl;
		cout << setprecision(10) << lBSO.highValue << endl;
		cout << setprecision(10) << rv << endl;
		cout << setprecision(10) << rBSO.highValue << endl;
		cout << setprecision(10) << BSO.highValue << endl;
		cout << setprecision(10) << feval<HIGHPRECISION>(lv, rv, op) << endl;
		cout << "---" << endl;
		exit(5);
	}

	assert(BSO.highValue == feval<HIGHPRECISION>(lv, rv, op));
	trace[iid][0] = BSO;
	computeBlameSummary(BSO, lBSO, rBSO, op);
}

void BlameAnalysis::call_lib(IID iid, IID argiid, HIGHPRECISION argv, MATHFUNC func) {
	const BlameShadowObject argBSO = getShadowObject(argiid, argv);
	const BlameShadowObject BSO = shadowFEval(iid, argBSO, func);
	trace[iid][0] = BSO;
	computeBlameSummary(BSO, argBSO, func);
}

void BlameAnalysis::fadd(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FADD);
}
void BlameAnalysis::fsub(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FSUB);
}
void BlameAnalysis::fmul(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FMUL);
}
void BlameAnalysis::fdiv(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fbinop(iid, liid, riid, lv, rv, FDIV);
}

void BlameAnalysis::call_sin(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, SIN);
}
void BlameAnalysis::call_acos(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, ACOS);
}
void BlameAnalysis::call_cos(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, COS);
}
void BlameAnalysis::call_fabs(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, FABS);
}
void BlameAnalysis::call_sqrt(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, SQRT);
}
void BlameAnalysis::call_log(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, LOG);
}
void BlameAnalysis::call_floor(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, FLOOR);
}
void BlameAnalysis::call_exp(IID iid, IID argIID, HIGHPRECISION argv) {
	call_lib(iid, argIID, argv, EXP);
}

void BlameAnalysis::fstore(IID iidV, void* vptr) {
	if (trace.find(iidV) != trace.end()) {
		trace[iidV][vptr] = trace[iidV][0];
		/*
		if (iidV == 3275) {
		  cout << "SD VALUE: " << trace[iidV][0].highValue << endl;
		  cout << "---" << endl;
		}
		*/
	}
}

void BlameAnalysis::fload(IID iidV, double v, IID iid, void* vptr) {
	if (trace.find(iid) != trace.end()) {
		trace[iidV][0] = trace[iid][vptr];
	} else {
		trace[iidV][0] = BlameShadowObject(iidV, (LOWPRECISION)v, v);
	}
	if (blameSummary.find(iid) != blameSummary.end()) {
		blameSummary[iidV] = blameSummary[iid];
	}
}

void BlameAnalysis::fphi(IID out, double v, IID in) {
	if (trace.find(in) != trace.end()) {
		trace[out][0] = trace[in][0];
	} else {
		trace[out][0] = BlameShadowObject(in, (LOWPRECISION)v, v);
	}
	if (blameSummary.find(in) != blameSummary.end()) {
		blameSummary[out] = blameSummary[in];
	}
}

void BlameAnalysis::fafter_call(IID iid, IID return_id) {
	if (trace.find(return_id) != trace.end()) {
		trace[iid][0] = trace[return_id][0];
	}
	if (blameSummary.find(return_id) != blameSummary.end()) {
		blameSummary[iid] = blameSummary[return_id];
	}
}

void BlameAnalysis::post_analysis() {
	std::ofstream tracefile;
	tracefile.open(_selfpath + ".trace");
	// Print the trace
	tracefile << "====== execution trace ======" << endl;
	for (auto& it : trace) {
		IID iid = it.first;
		DebugInfo dbg = debugInfoMap.at(iid);
		BlameShadowObject bso = it.second[0];
		tracefile << "At file " << dbg.file << ", line " << dbg.line << ", column " << dbg.column << ", id " << iid << endl;
		tracefile << "\t" << setprecision(10) << bso.lowValue << ", " << bso.highValue << endl;
	}

	std::ofstream logfile;
	std::ofstream logfile2;
	logfile.open(_selfpath + ".ba");
	logfile2.open(_selfpath + ".ba.full");

	// Read _iid from file or using the default starting point
	ifstream fin(_selfpath + ".point");
	if (fin.fail()) {
		logfile << "File with starting point does not exist." << endl;
		logfile << "Using the default starting point." << endl;
	} else {
		fin >> _iid;
	}

	DebugInfo dbg = debugInfoMap.at(_iid);
	logfile << "Default starting point: File " << dbg.file << ", Line " << dbg.line << ", Column " << dbg.column
			<< ", IID " << _iid << "\n";
	logfile << "Default precision: " << PRECISION_BITS[_precision] << "\n";

	std::set<BlameNode> visited;
	std::queue<BlameNode> workList;
	workList.push(blameSummary[_iid][_precision]);

	while (!workList.empty()) {
		// Find more blame node and add to the queue.
		const BlameNode& node = workList.front();
		logfile2 << "(" << node.id.iid << "," << PRECISION_BITS[node.id.precision] << ") : ";
		for (auto it = node.children.begin(); it != node.children.end(); it++) {
			BlameNodeID blameNodeID = *it;
			logfile2 << "(" << blameNodeID.iid << "," << PRECISION_BITS[blameNodeID.precision] << ") ";
			if (blameSummary.find(blameNodeID.iid) == blameSummary.end()) {
				// Children are either a constant or alloca.
				continue;
			}
			const BlameNode& blameNode = blameSummary[blameNodeID.iid][blameNodeID.precision];
			if (visited.find(blameNode) == visited.end()) {
				visited.insert(blameNode);
				workList.push(blameNode);
			}
		}
		logfile2 << endl;
		workList.pop();

		// Interpret the result for the current blame node.
		if (debugInfoMap.find(node.id.iid) == debugInfoMap.end()) {
			continue;
		}
		DebugInfo dbg = debugInfoMap.at(node.id.iid);
		if (node.requireHigherPrecision || node.requireHigherPrecisionOperator) {
			logfile << "File " << dbg.file << ", Line " << dbg.line << ", Column " << dbg.column
					<< ", HigherPrecision: " << node.requireHigherPrecision
					<< ", HigherPrecisionOperator: " << node.requireHigherPrecisionOperator << "\n";
		}
	}

	logfile.close();
}
