// Author: Cuong Nguyen

#include "BlameAnalysis.h"
#include "../src/InstructionMonitor.h"

/******* ANALYSIS PARAMETERS *******/
vector<vector<BlameTreeShadowObject<HIGHPRECISION>>> BlameAnalysis::trace;
map<uint64_t, DebugInfo> BlameAnalysis::debugInfoMap;
int BlameAnalysis::dpc = 0;

/******* HELPER FUNCTIONS *******/

void BlameAnalysis::copyShadow(IValue* src, IValue* dest) {
	if (src->getShadow() != NULL) {
		BlameTreeShadowObject<HIGHPRECISION>* btmSOSrc =
			(BlameTreeShadowObject<HIGHPRECISION>*)src->getShadow();
		BlameTreeShadowObject<HIGHPRECISION>* btmSODest =
			new BlameTreeShadowObject<HIGHPRECISION>(*btmSOSrc);
		dest->setShadow(btmSODest);
	} else {
		dest->setShadow(NULL);
	}
}

void BlameAnalysis::setShadowObject(
	SCOPE scope, int64_t inx,
	BlameTreeShadowObject<HIGHPRECISION>* shadowObject) {
	IValue* iv = NULL;

	switch (scope) {
		case CONSTANT:
			return;
		case GLOBAL:
			iv = globalSymbolTable[inx];
			break;
		case LOCAL:
			iv = executionStack.top()[inx];
			break;
		default:
			DEBUG_STDERR("Unknown scope " << scope);
			safe_assert(false);
	}

	iv->setShadow(shadowObject);
}

BlameTreeShadowObject<HIGHPRECISION>*
BlameAnalysis::getShadowObject(SCOPE scope, int64_t inx) {
	IValue* iv = NULL;

	switch (scope) {
		case CONSTANT:
			return NULL;
		case GLOBAL:
			iv = globalSymbolTable[inx];
			break;
		case LOCAL:
			iv = executionStack.top()[inx];
			break;
		default:
			DEBUG_STDERR("Unknown scope " << scope);
			safe_assert(false);
	}

	return (BlameTreeShadowObject<HIGHPRECISION>*)iv->getShadow();
}

HIGHPRECISION BlameAnalysis::getShadowValue(SCOPE scope, int64_t inx,
		PRECISION precision) {
	HIGHPRECISION result;
	IValue* iv = NULL;
	double* ptr;

	switch (scope) {
		case CONSTANT:
			ptr = (double*)&inx;
			result = *ptr;
			return result;
		case GLOBAL:
			iv = globalSymbolTable[inx];
			break;
		case LOCAL:
			iv = executionStack.top()[inx];
			break;
		default:
			DEBUG_STDERR("Unknown scope " << scope);
			safe_assert(false);
	}

	if (iv->getShadow() == NULL) {
		result = iv->getFlpValue();
	} else {
		result = ((BlameTreeShadowObject<HIGHPRECISION>*)iv->getShadow())
				 ->getValue(precision);
	}

	return result;
}

LOWPRECISION BlameAnalysis::getActualValue(SCOPE scope, int64_t value) {
	LOWPRECISION actualValue;
	IValue* iv = NULL;
	double* ptr;

	switch (scope) {
		case CONSTANT:
			ptr = (double*)&value;
			actualValue = *ptr;
			return actualValue;
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

	actualValue = iv->getFlpValue();
	return actualValue;
}

/******* ANALYSIS FUNCTIONS *******/

void BlameAnalysis::pre_analysis() {
	// Read debug information from $GLOG_log_dir/debug.bin.
	std::string debugFileName(getenv("GLOG_log_dir"));
	debugFileName += "/debug.bin";

	FILE* debugFile = fopen(debugFileName.c_str(), "rb");
	struct DebugInfo debugInfo;
	uint64_t iid;

	while (fread(&iid, sizeof(uint64_t), 1, debugFile) &&
			fread(&debugInfo, sizeof(struct DebugInfo), 1, debugFile)) {
		std::cout << iid << ": " << debugInfo.file << ", " << debugInfo.line
				  << std::endl;
		debugInfoMap[iid] = debugInfo;
	}
	fclose(debugFile);

	// Set copy shadow function for blame analysis.
	IValue::setCopyShadow(&copyShadow);
}

void BlameAnalysis::post_lib_call(IID iid UNUSED, bool nounwind UNUSED, int pc,
								  KIND type UNUSED, int inx, SCOPE argScope,
								  int64_t argValueOrIndex, string func) {
	PRECISION p;

	// Obtain actual values and shadow values.
	LOWPRECISION arg = getActualValue(argScope, argValueOrIndex);
	LOWPRECISION result = executionStack.top()[inx]->getFlpValue();

	BlameTreeShadowObject<HIGHPRECISION>* shadow =
		getShadowObject(argScope, argValueOrIndex);

	// Perform library function on shadow values.
	HIGHPRECISION values[PRECISION_NO];

	// Shadow object for operands.
	if (!shadow) {
		// Constructing and setting shadow object.
		HIGHPRECISION values[PRECISION_NO];
		PRECISION p;
		values[BITS_FLOAT] = arg;
		values[BITS_DOUBLE] = (HIGHPRECISION) arg;
		for (p = PRECISION(BITS_FLOAT + 1); p < BITS_DOUBLE; p = PRECISION(p + 1)) {
			values[p] = BlameTreeUtilities::clearBits(
							(HIGHPRECISION) arg, 52 - BlameTreeUtilities::exactBits(p));
		}

		shadow = new BlameTreeShadowObject<HIGHPRECISION>(
			0, pc, 0, dpc, CONSTANT_INTR, BINOP_INVALID, "NONE", values);
		setShadowObject(argScope, argValueOrIndex, shadow);
	}

	// retrieve the value in higher precision
	HIGHPRECISION sarg = shadow->getValue(BITS_DOUBLE);
	HIGHPRECISION sresult = BlameTreeUtilities::evalFunc(sarg, func);

	values[BITS_FLOAT] = result;
	values[BITS_DOUBLE] = sresult;
	for (p = PRECISION(BITS_FLOAT + 1); p < BITS_DOUBLE; p = PRECISION(p + 1)) {
		values[p] = BlameTreeUtilities::clearBits(
						sresult, 52 - BlameTreeUtilities::exactBits(p));
	}

	// creating shadow object for the result
	BlameTreeShadowObject<HIGHPRECISION>* resultShadow =
		new BlameTreeShadowObject<HIGHPRECISION>(0, pc, 0, dpc, CALL_INTR,
				BINOP_INVALID, func, values);
	executionStack.top()[inx]->setShadow(resultShadow);

	// adding to the trace
	vector<BlameTreeShadowObject<HIGHPRECISION>> shadows;
	shadows.push_back(*resultShadow);
	shadows.push_back(*shadow);
	trace.push_back(shadows);

	dpc++;
	return;

}

void BlameAnalysis::post_call_sin(IID iid UNUSED, bool nounwind UNUSED, int pc,
								  KIND type UNUSED, int inx, SCOPE argScope,
								  int64_t argValueOrIndex) {
	post_lib_call(iid, nounwind, pc, type, inx, argScope, argValueOrIndex, "sin");
}

void BlameAnalysis::post_call_cos(IID iid UNUSED, bool nounwind UNUSED, int pc,
								  KIND type UNUSED, int inx, SCOPE argScope,
								  int64_t argValueOrIndex) {
	post_lib_call(iid, nounwind, pc, type, inx, argScope, argValueOrIndex, "cos");
}

void BlameAnalysis::post_call_acos(IID iid UNUSED, bool nounwind UNUSED, int pc,
								   KIND type UNUSED, int inx, SCOPE argScope,
								   int64_t argValueOrIndex) {
	post_lib_call(iid, nounwind, pc, type, inx, argScope, argValueOrIndex,
				  "acos");
}

void BlameAnalysis::post_call_sqrt(IID iid UNUSED, bool nounwind UNUSED, int pc,
								   KIND type UNUSED, int inx, SCOPE argScope,
								   int64_t argValueOrIndex) {
	post_lib_call(iid, nounwind, pc, type, inx, argScope, argValueOrIndex,
				  "sqrt");
}

void BlameAnalysis::post_call_fabs(IID iid UNUSED, bool nounwind UNUSED, int pc,
								   KIND type UNUSED, int inx, SCOPE argScope,
								   int64_t argValueOrIndex) {
	post_lib_call(iid, nounwind, pc, type, inx, argScope, argValueOrIndex,
				  "fabs");
}

void BlameAnalysis::post_call_log(IID iid UNUSED, bool nounwind UNUSED, int pc,
								  KIND type UNUSED, int inx, SCOPE argScope,
								  int64_t argValueOrIndex) {
	post_lib_call(iid, nounwind, pc, type, inx, argScope, argValueOrIndex, "log");
}

void BlameAnalysis::post_call_floor(IID iid UNUSED, bool nounwind UNUSED,
									int pc, KIND type UNUSED, int inx,
									SCOPE argScope, int64_t argValueOrIndex) {
	post_lib_call(iid, nounwind, pc, type, inx, argScope, argValueOrIndex,
				  "floor");
}

void BlameAnalysis::post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue,
								int64_t rValue, KIND type, int file, int line,
								int col, int inx UNUSED, BINOP op) {

	BlameTreeShadowObject<HIGHPRECISION>* s1, *s2;
	HIGHPRECISION sv1, sv2, sresult = 0.0;
	LOWPRECISION v1, v2;
	PRECISION i;

	// Assert: type is a floating-point type.
	safe_assert(type == FLP32_KIND || type == FLP64_KIND ||
				type == FLP80X86_KIND || type == FLP128_KIND ||
				type == FLP128PPC_KIND);

	// Obtain actual values, and shadow values.
	v1 = getActualValue(lScope, lValue);
	v2 = getActualValue(rScope, rValue);

	s1 = getShadowObject(lScope, lValue);
	s2 = getShadowObject(rScope, rValue);

	// Perform binary operation for shadow values.
	HIGHPRECISION values[PRECISION_NO];

	// Shadow objects for operands.
	if (!s1) {
		// Constructing and setting shadow object.
		HIGHPRECISION values[PRECISION_NO];
		PRECISION p;
		values[BITS_FLOAT] = v1;
		values[BITS_DOUBLE] = (HIGHPRECISION) v1;
		for (p = PRECISION(BITS_FLOAT + 1); p < BITS_DOUBLE; p = PRECISION(p + 1)) {
			values[p] = BlameTreeUtilities::clearBits(
							(HIGHPRECISION) v1, 52 - BlameTreeUtilities::exactBits(p));
		}

		s1 = new BlameTreeShadowObject<HIGHPRECISION>(
			file, line, col, dpc, CONSTANT_INTR, BINOP_INVALID, "NONE", values);
		setShadowObject(lScope, lValue, s1);
	}

	if (!s2) {
		// constructing and setting shadow object
		HIGHPRECISION values[PRECISION_NO];
		PRECISION p;
		values[BITS_FLOAT] = v2;
		values[BITS_DOUBLE] = (HIGHPRECISION) v2;
		for (p = PRECISION(BITS_FLOAT + 1); p < BITS_DOUBLE; p = PRECISION(p + 1)) {
			values[p] = BlameTreeUtilities::clearBits(
							(HIGHPRECISION) v2, 52 - BlameTreeUtilities::exactBits(p));
		}

		s2 = new BlameTreeShadowObject<HIGHPRECISION>(
			file, line, col, dpc, CONSTANT_INTR, BINOP_INVALID, "NONE", values);
		setShadowObject(rScope, rValue, s2);
	}

	// retrieve values in higher precision
	sv1 = s1->getValue(BITS_DOUBLE);
	sv2 = s2->getValue(BITS_DOUBLE);

	switch (op) {
		case FADD:
			sresult = sv1 + sv2;
			break;
		case FSUB:
			sresult = sv1 - sv2;
			break;
		case FMUL:
			sresult = sv1 * sv2;
			break;
		case FDIV:
			sresult = sv1 / sv2;
			break;
		default:
			DEBUG_STDERR(
				"Unsupported floating-point binary operator: " << BINOP_ToString(op));
			safe_assert(false);
	}

	// truncate result depending on precision
	values[BITS_FLOAT] = executionStack.top()[inx]->getFlpValue();
	values[BITS_DOUBLE] = sresult;
	for (i = PRECISION(BITS_FLOAT + 1); i < BITS_DOUBLE; i = PRECISION(i + 1)) {
		values[i] = BlameTreeUtilities::clearBits(
						sresult, 52 - BlameTreeUtilities::exactBits(i));
	}

	// creating shadow object for target
	BlameTreeShadowObject<HIGHPRECISION>* resultShadow =
		new BlameTreeShadowObject<HIGHPRECISION>(file, line, col, dpc, BIN_INTR,
				op, "NONE", values);
	executionStack.top()[inx]->setShadow(resultShadow);

	// adding to the trace
	vector<BlameTreeShadowObject<HIGHPRECISION>> shadows;
	shadows.push_back(*resultShadow);
	shadows.push_back(*s1);
	shadows.push_back(*s2);
	trace.push_back(shadows);

	dpc++;
	return;
}

void BlameAnalysis::post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue,
							  int64_t rValue, KIND type, int file, int line,
							  int col, int inx) {
	post_fbinop(lScope, rScope, lValue, rValue, type, file, line, col, inx, FADD);
}

void BlameAnalysis::post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue,
							  int64_t rValue, KIND type, int file, int line,
							  int col, int inx) {
	post_fbinop(lScope, rScope, lValue, rValue, type, file, line, col, inx, FSUB);
}

void BlameAnalysis::post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue,
							  int64_t rValue, KIND type, int file, int line,
							  int col, int inx) {
	post_fbinop(lScope, rScope, lValue, rValue, type, file, line, col, inx, FMUL);
}

void BlameAnalysis::post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue,
							  int64_t rValue, KIND type, int file, int line,
							  int col, int inx) {
	post_fbinop(lScope, rScope, lValue, rValue, type, file, line, col, inx, FDIV);
}

void BlameAnalysis::post_fptrunc(int64_t op, SCOPE opScope, KIND opKind UNUSED,
								 KIND kind UNUSED, int size UNUSED, int inx) {
	BlameTreeShadowObject<HIGHPRECISION>* opbtSO = getShadowObject(opScope, op);

	if (opbtSO) {
		BlameTreeShadowObject<HIGHPRECISION>* btSO =
			new BlameTreeShadowObject<HIGHPRECISION>(*opbtSO);
		executionStack.top()[inx]->setShadow(btSO);
	}
}

void BlameAnalysis::post_fpext(int64_t op, SCOPE opScope, KIND opKind UNUSED,
							   KIND kind UNUSED, int size UNUSED, int inx) {
	BlameTreeShadowObject<HIGHPRECISION>* opbtSO = getShadowObject(opScope, op);

	if (opbtSO) {
		BlameTreeShadowObject<HIGHPRECISION>* btSO =
			new BlameTreeShadowObject<HIGHPRECISION>(*opbtSO);
		executionStack.top()[inx]->setShadow(btSO);
	}
}

void BlameAnalysis::post_analysis() {
	cout << "Printing trace after analysis: " << dpc << endl;
	cout << "Trace length: " << dpc << endl;
	std::string line;

	//
	// Print the trace
	//
	cout << endl;
	cout << "Do you want to see the trace (yes|no)?" << endl;
	std::getline(std::cin, line);

	if (line.compare("yes") == 0) {
		for (int i = max(0, dpc - 500); i < dpc; i++) {
			cout << "DPC: " << i << endl;
			for (unsigned j = 0; j < trace[i].size(); j++) {
				cout << "\t";
				trace[i][j].print();
			}
		}
	}

	//
	// read rootnode from a file
	//
	cout << endl;
	cout << "Ok, now you have seen the trace, take a look at all computation"
		 << "points with values with different precision." << endl;
	cout << "Tell me:" << endl;
	cout << "\t Which computation point you are interested in?" << endl;
	cout << "\t What is your desired precision for that computation point?"
		 << endl;

	cout << endl;
	cout << "I suggest the following parameter" << endl;
	cout << "\t Computation point: " << (dpc - 1) << endl;
	cout << "\t Desired precision: exact to "
		 << BlameTreeUtilities::precisionToString(PRECISION(PRECISION_NO / 2))
		 << endl;

	BlameNodeID rootNode(0, BITS_FLOAT);
	std::getline(std::cin, line);

	if (line.empty()) {
		rootNode = BlameNodeID(dpc - 1, PRECISION(PRECISION_NO / 2));
	} else {
		std::stringstream lineStream(line);
		std::string token;
		int dpc, prec;

		lineStream >> token;
		dpc = atoi(token.c_str());
		lineStream >> token;
		prec = atoi(token.c_str());

		rootNode = BlameNodeID(dpc, BlameTreeUtilities::exactBitToPrecision(prec));
	}

	//
	// construct blame tree
	//
	cout << endl;
	cout << "Constructing blame tree from root at:" << endl;
	cout << "\tComputation point: " << rootNode.dpc << endl;
	cout << "\tDesired precision: " << BlameTreeUtilities::precisionToString(
			 rootNode.precision) << endl;

	BlameTree bta(rootNode);
	clock_t startTime, endTime;

	startTime = clock();

	bta.constructBlameGraph(trace);

	endTime = clock();

	cout << endl;
	cout << "Done!" << endl;
	cout << "Construction time: " << double(endTime - startTime) /
		 double(CLOCKS_PER_SEC) << " seconds."
		 << endl;
	cout << "Analysis result:" << endl;

	bta.printResult();

	cout << endl;
	cout << "Do you want to visualize the blame tree? (yes|no)" << endl;
	std::getline(std::cin, line);

	if (line.compare("yes") == 0) {
		cout << endl;
		cout << "Blame tree in dot format: blametree.dot" << endl;
		cout << endl;

		ofstream blametree;
		blametree.open("blametree.dot");
		blametree << bta.toDot();
		blametree.close();
	}

	return;
}

static RegisterObserver<BlameAnalysis> BlameAnalysisInstance("blameanalysis");
