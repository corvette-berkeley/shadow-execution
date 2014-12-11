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
		//    cout << "IID: " << iid << " is a constant." << endl;
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

BlameNode BlameAnalysis::computeBlameInformation(const BlameShadowObject& BSO, const BlameShadowObject& lBSO,
		const BlameShadowObject& rBSO, PRECISION p) {
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
			if (!equalWithinPrecision(
						val, clearBits(pow(lbsoVals[i], rbsoVals[j]), DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[p]), p)) {
				continue;
			}

			found = true;
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

inline void BlameAnalysis::copyShadowObject(IID dstIID, void* dstPtr, IID srcIID, void* srcPtr, double v) {
	if (trace.find(srcIID) != trace.end() && trace[srcIID].find(srcPtr) != trace[srcIID].end()) {
		BlameShadowObject bso = trace[srcIID][srcPtr];
		trace[dstIID][dstPtr] = BlameShadowObject(dstIID, bso.lowValue, bso.highValue);
	} else {
		trace[dstIID][dstPtr] = BlameShadowObject(dstIID, (LOWPRECISION)v, v);
	}
}

inline void BlameAnalysis::copyBlameSummary(IID dest, IID src) {
	alias[dest].insert(src);
}

inline void BlameAnalysis::computeDivergeNode(const BlameShadowObject& lBSO, const BlameShadowObject& rBSO, CMPOP op) {
	bool truthVal = fcmp_eval<HIGHPRECISION>(lBSO.highValue, rBSO.highValue, op);
	for (PRECISION i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
		for (PRECISION j = BITS_FLOAT; j < PRECISION_NO; j = PRECISION(j + 1)) {
			double left =
				(i != BITS_FLOAT) ? clearBits(lBSO.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[i]) : lBSO.lowValue;
			double right =
				(j != BITS_FLOAT) ? clearBits(rBSO.highValue, DOUBLE_MANTISSA_LENGTH - PRECISION_BITS[j]) : rBSO.lowValue;
			if (fcmp_eval<HIGHPRECISION>(left, right, op) == truthVal) {
				diverge.insert(BlameNodeID(lBSO.id, i));
				diverge.insert(BlameNodeID(rBSO.id, j));
				return;
			}
		}
	}
}

/*** API FUNCTIONS ***/
void BlameAnalysis::fcmp(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv, CMPOP op) {
	if (!startTrack(iid)) {
		return;
	}

	const BlameShadowObject lBSO = getShadowObject(liid, lv);
	const BlameShadowObject rBSO = getShadowObject(riid, rv);
	if (fcmp_eval<HIGHPRECISION>(lBSO.highValue, rBSO.highValue, op) !=
			fcmp_eval<LOWPRECISION>(lBSO.lowValue, rBSO.lowValue, op)) {
		/*
		   cout << "Divergence happend." << endl;
		   cout << "IID: " << iid << endl;
		   cout << "RIID: " << riid << endl;
		   cout << "R high: " << rBSO.highValue << " R low: " << rBSO.lowValue << endl;
		   cout << "LIID: " << liid << endl;
		   cout << "L high: " << lBSO.highValue << " L low: " << lBSO.lowValue << endl;
		   cout << "----" << endl;
		   */
		//    exit(5);
		computeDivergeNode(lBSO, rBSO, op);
	}
}

void BlameAnalysis::fbinop(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv, FBINOP op) {
	if (!startTrack(iid)) {
		return;
	}
	const BlameShadowObject lBSO = getShadowObject(liid, lv);
	const BlameShadowObject rBSO = getShadowObject(riid, rv);
	const BlameShadowObject BSO = shadowFEval(iid, lBSO, rBSO, op);

	if ((BSO.highValue != feval<HIGHPRECISION>(lv, rv, op))) {
		cout << "IID: " << iid << endl;
		cout << "RIID: " << riid << endl;
		cout << "LIID: " << liid << endl;
		cout << setprecision(20) << lv << endl;
		cout << setprecision(20) << lBSO.highValue << endl;
		cout << setprecision(20) << lBSO.lowValue << endl;
		cout << setprecision(20) << rv << endl;
		cout << setprecision(20) << rBSO.highValue << endl;
		cout << setprecision(20) << rBSO.lowValue << endl;
		cout << setprecision(20) << feval<HIGHPRECISION>(lv, rv, op) << endl;
		cout << setprecision(20) << BSO.highValue << endl;
		cout << setprecision(20) << BSO.lowValue << endl;
		cout << "---" << endl;
		exit(5);
	}

	assert(BSO.highValue == feval<HIGHPRECISION>(lv, rv, op));
	trace[iid][0] = BSO;
	computeBlameSummary(BSO, lBSO, rBSO, op);
}

void BlameAnalysis::call_lib(IID iid, IID argiid, HIGHPRECISION argv, MATHFUNC func) {
	if (!startTrack(iid)) {
		return;
	}
	const BlameShadowObject argBSO = getShadowObject(argiid, argv);
	const BlameShadowObject BSO = shadowFEval(iid, argBSO, func);
	trace[iid][0] = BSO;
	computeBlameSummary(BSO, argBSO, func);
}

void BlameAnalysis::call_pow(IID iid, IID argiid01, HIGHPRECISION argv01, IID argiid02, HIGHPRECISION argv02) {
	if (!startTrack(iid)) {
		return;
	}
	const BlameShadowObject argBSO01 = getShadowObject(argiid01, argv01);
	const BlameShadowObject argBSO02 = getShadowObject(argiid02, argv02);

	// shadow function eval
	const BlameShadowObject BSO =
		BlameShadowObject(iid, pow(argBSO01.lowValue, argBSO02.lowValue), pow(argBSO01.highValue, argBSO02.highValue));
	trace[iid][0] = BSO;

	// compute blame summary
	std::array<BlameNode, PRECISION_NO> blames;
	blames[BITS_FLOAT] = BlameNode(BSO.id, BITS_FLOAT, false, false,
	{BlameNodeID(argBSO01.id, BITS_FLOAT), BlameNodeID(argBSO02.id, BITS_FLOAT)});

	for (PRECISION p = PRECISION(BITS_FLOAT + 1); p < PRECISION_NO; p = PRECISION(p + 1)) {
		blames[p] = computeBlameInformation(BSO, argBSO01, argBSO02, p);
	}

	blameSummary[BSO.id] = blames;
}

void BlameAnalysis::oeq(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fcmp(iid, liid, riid, lv, rv, OEQ);
}

void BlameAnalysis::ogt(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fcmp(iid, liid, riid, lv, rv, OGT);
}

void BlameAnalysis::oge(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fcmp(iid, liid, riid, lv, rv, OGE);
}

void BlameAnalysis::olt(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fcmp(iid, liid, riid, lv, rv, OLT);
}

void BlameAnalysis::ole(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fcmp(iid, liid, riid, lv, rv, OLE);
}

void BlameAnalysis::one(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv) {
	fcmp(iid, liid, riid, lv, rv, ONE);
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

void BlameAnalysis::fstore(IID iidV, IID, void* vptr) {
	//  if (!startTrack(iid)) {
	//    return;
	//  }
	if (trace.find(iidV) != trace.end()) {
		trace[iidV][vptr] = trace[iidV][0];
	}
}

void BlameAnalysis::fload(IID iidV, double v, IID iid, void* vptr) {
	//  if (!startTrack(iidV)) {
	//    return;
	//  }
	copyShadowObject(iidV, 0, iid, vptr, v);
	copyBlameSummary(iidV, iid);
}

void BlameAnalysis::fphi(IID out, double v, IID in) {
	if (!startTrack(out)) {
		return;
	}
	copyShadowObject(out, 0, in, 0, v);
	copyBlameSummary(out, in);
}

void BlameAnalysis::fafter_call(IID iid, double v, IID return_id) {
	if (!startTrack(iid)) {
		return;
	}
	copyShadowObject(iid, 0, return_id, 0, v);
	copyBlameSummary(iid, return_id);
}

void BlameAnalysis::dumpTrace() {
	std::ofstream tracefile;
	tracefile.open(_selfpath + ".trace");
	for (auto& it : trace) {
		IID iid = it.first;
		DebugInfo dbg = debugInfoMap.at(iid);
		BlameShadowObject bso = it.second[0];
		tracefile << "At file " << dbg.file << ", line " << dbg.line << ", column" << dbg.column << ", id " << iid << endl;
		tracefile << bso.lowValue << ", " << bso.highValue << endl;
	}
}

void BlameAnalysis::constructAliasBlame() {
	for (auto it : alias) {
		IID first = it.first;
		std::set<IID> second = it.second;

		// find a close set of aliases
		std::set<IID> closeset;
		std::queue<IID> workList;
		workList.push(first);
		while (!workList.empty()) {
			IID id = workList.front();
			workList.pop();
			if (alias.find(id) == alias.end()) {
				continue;
			}
			std::set<IID> al = alias[id];
			for (auto iid : al) {
				if (closeset.find(iid) == closeset.end()) {
					closeset.insert(iid);
					workList.push(iid);
				}
			}
		}

		for (PRECISION p = BITS_FLOAT; p < PRECISION_NO; p = PRECISION(p + 1)) {
			std::vector<BlameNodeID> bnids;
			bool requireHigherPrecision = false;
			bool requireHigherPrecisionOperator = false;
			for (auto iid : second) {
				bnids.push_back(BlameNodeID(iid, p));
			}
			for (auto iid : closeset) {
				if (blameSummary.find(iid) == blameSummary.end()) {
					continue;
				}
				requireHigherPrecision = requireHigherPrecision || blameSummary[iid][p].requireHigherPrecision;
				requireHigherPrecisionOperator =
					requireHigherPrecisionOperator || blameSummary[iid][p].requireHigherPrecisionOperator;
			}
			blameSummary[first][p] = BlameNode(first, p, requireHigherPrecision, requireHigherPrecisionOperator, bnids);
		}
	}
}

void BlameAnalysis::pre_analysis() {
	total_inst_count = 0;
	inst_count = 0;
	ifstream cin(_selfpath + ".ic");
	if (cin.fail()) {
		cout << "Instruction counter file does not exist." << endl;
		cout << "Compute blames for all instruction instances." << endl;
		return;
	}
	while (cin) {
		IID iid;
		uint64_t count;
		cin >> iid >> count;
		total_inst_count += count;
		/*
		if (count > 2000) {
		  start[iid] = count - 1000;
		}
		*/
	}
}

void BlameAnalysis::post_analysis() {
	// dumpTrace()
	constructAliasBlame();

	// obtain all starting points
	vector<IID> starts;
	ifstream pin(_selfpath + ".point");
	if (pin.fail()) {
		cout << "File with starting point does not exist." << endl;
		cout << "Using the default starting point." << endl;
		starts.push_back(_iid);
	} else {
		int start;
		while (pin >> start) {
			starts.push_back(start);
		}
	}
	pin.close();

	// obtain all precisions
	vector<PRECISION> precisions;
	ifstream prin(_selfpath + ".precision");
	if (prin.fail()) {
		cout << "File with precisions does not exist." << endl;
		cout << "Using the default precision." << endl;
		precisions.push_back(_precision);
	} else {
		int decimal;
		while (prin >> decimal) {
			switch (decimal) {
				case 10:
					precisions.push_back(BITS_33);
					break;
				case 8:
					precisions.push_back(BITS_27);
					break;
				case 6:
					precisions.push_back(BITS_19);
					break;
				case 4:
					precisions.push_back(BITS_13);
					break;
				default:
					break;
			}
		}
	}

	for (PRECISION p : precisions) {
		std::ofstream logfile;
		std::ofstream logfile2;
		logfile.open(_selfpath + "_" + std::to_string(PRECISION_BITS[p]) + ".ba");
		logfile2.open(_selfpath + "_" + std::to_string(PRECISION_BITS[p]) + ".ba.full");

		for (IID iid : starts) {
			DebugInfo dbg = debugInfoMap.at(iid);
			logfile << "Default starting point: File " << dbg.file << ", Line " << dbg.line << ", Column " << dbg.column
					<< ", IID " << iid << "\n";
			logfile << "Default precision: " << PRECISION_BITS[p] << "\n";
		}


		// Interpreting results
		std::set<BlameNode> visited;
		std::queue<BlameNode> workList;
		for (IID iid : starts) {
			workList.push(blameSummary[iid][p]);
		}
		for (auto& nodeid : diverge) {
			workList.push(blameSummary[nodeid.iid][nodeid.precision]);  // Diverge nodes prevent divergence
		}

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
				logfile << "File " << dbg.file << ", Line " << dbg.line << ", Column " << dbg.column << endl;
				//						<< ", HigherPrecision: " << node.requireHigherPrecision
				//						<< ", HigherPrecisionOperator: " << node.requireHigherPrecisionOperator << "\n";
			}
		}

		logfile.close();
		logfile2.close();
	}
}
