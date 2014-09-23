// Author: Cuong Nguyen

#ifndef BLAME_ANALYSIS_H_
#define BLAME_ANALYSIS_H_

#include <unordered_map>
#include <cmath>

#include "../../src/IValue.h"
#include "../../src/InterpreterObserver.h"

#include "BlameNode.h"
#include "BlameShadowObject.h"

class BlameAnalysis : public InterpreterObserver {

private:
	// Return the file separator character depending on the underlying operating
	// system.
	inline char separator() {
#ifdef _WIN32
		return '\\';
#else
		return '/';
#endif
	}

	// Read debug information from $GLOG_log_dir/debug.bin and wrap into a
	// mapping from instruction IID to DebugInfo struct. The file debug.bin is
	// constructed during instrumentation phase.
	//
	// TODO: find a way to make the name debug.bin to be provided as input and
	// sounds more personal to the application under analysis.
	unordered_map<IID, DebugInfo> readDebugInfo();

	// A map from instruction IID to debug information. IID is computed during
	// instrumentation phase and is the unique id for each LLVM instruction.
	// Debug information includes the LoC, column and file of the instruction.
	const unordered_map<IID, DebugInfo> debugInfoMap = readDebugInfo();

	// A map from instruction IID to blame summary. IID is computed during
	// instrumentation phase and is the unique id for each LLVM instruction.
	// Blame summary is a tree-like data structure which represents the precision
	// dependency of values used and defined in this instruction.
	map<IID, BlameNode[PRECISION_NO]> blameSummary;

public:
	BlameAnalysis(std::string name) : InterpreterObserver(name) {}

	/*** API FUNCTIONS ***/
	virtual void pre_analysis();

	virtual void post_fadd(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_fsub(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_fmul(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_fdiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_analysis();

private:
	// Replace the last "shift" bits of the double value with 0 bits.
	double clearBits(double v, int shift);

	// Verify whether the two double values v1 and v2 equal within the precision
	// given.
	//
	// This function first clears the last few bits of v1 and v2 to match
	// with the precision and then compares the resulting values. When comparing
	// the resulting values, inequality due to rounding is torelated.
	bool equalWithinPrecision(double v1, double v2, PRECISION prec);

	template <typename T> T feval(T val01, T val02, BINOP bop);

	// Get the shadow object of an LLVM instruction. An LLVM instruction is
	// identified by its iid, scope and value or index.
	const BlameShadowObject getShadowObject(IID iid, SCOPE scope, int64_t value);

	void copyShadow(IValue* src, IValue* dest);

	void post_fbinop(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type,
					 int inx, BINOP op);
};
#endif
