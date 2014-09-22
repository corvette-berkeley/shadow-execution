// Author: Cuong Nguyen

#ifndef BLAME_ANALYSIS_H_
#define BLAME_ANALYSIS_H_

#include <unordered_map>
#include "../../src/Common.h"
#include "../../src/InterpreterObserver.h"

class BlameAnalysis : public InterpreterObserver {

private:
	inline char separator() {
#ifdef _WIN32
		return '\\';
#else
		return '/';
#endif
	}

	unordered_map<IID, DebugInfo> readDebugInfo() {
		// Read debug information from $GLOG_log_dir/debug.bin and store into
		// debugInfoMap.
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

	const unordered_map<IID, DebugInfo> debugInfoMap = readDebugInfo();

public:
	BlameAnalysis(std::string name) : InterpreterObserver(name) {};
};

#endif
