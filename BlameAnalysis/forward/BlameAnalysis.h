// Author: Cuong Nguyen

#ifndef BLAME_ANALYSIS_H_
#define BLAME_ANALYSIS_H_

#include <unordered_map>

#include "BlameNode.h"
#include "BlameShadowObject.h"
#include "../../src/IValue.h"
#include "../../src/InterpreterObserver.h"

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
	unordered_map<IID, DebugInfo> readDebugInfo() {
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

	virtual void post_fadd(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx);

	virtual void post_fsub(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx);

	virtual void post_fmul(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx);

	virtual void post_fdiv(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx);

	virtual void post_analysis();

private:
	void post_fbinop(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx, BINOP op);

	void copyShadow(IValue* src, IValue* dest);

	const BlameShadowObject getShadowObject(SCOPE scope, int64_t value);
};
#endif
