// Author: Cuong Nguyen

#ifndef BLAME_ANALYSIS_H_
#define BLAME_ANALYSIS_H_

#include <unordered_map>
#include "../../src/Common.h"
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
	const unordered_map<IID, DebugInfo> debugInfoMap = readDebugInfo();

public:
	BlameAnalysis(std::string name) : InterpreterObserver(name) {};
};

#endif
