#ifndef _BLAME_ANALYSIS_H_
#define _BLAME_ANALYSIS_H_

#include <unordered_map>
#include <map>

#include "BlameUtilities.h"
#include "BlameNode.h"
#include "BlameShadowObject.h"

using std::unordered_map;
using std::string;

class BlameAnalysis {
private:
	// Return the file separator character depending on the underlying operating
	// system.
	inline char separator() {
#ifdef _WIN32
		return '\\';
#else
		return '/';
#endif
	};

	string get_selfpath();

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

	unordered_map<IID, unordered_map<void*, BlameShadowObject>> trace;
	unordered_map<IID, std::array<BlameNode, PRECISION_NO>> blameSummary;

	// Global information about the starting point of the analysis.
	PRECISION _precision;
	IID _iid;
	string _selfpath;

	BlameAnalysis() {
		_precision = BITS_27;
		_iid = 0;
		_selfpath = get_selfpath();
	}

public:
	static BlameAnalysis& get() {
		static BlameAnalysis global;
		return global;
	}

	~BlameAnalysis() {
		post_analysis();
	}

	void call_sin(IID iid, IID argIID, HIGHPRECISION argv);
	void call_acos(IID iid, IID argIID, HIGHPRECISION argv);
	void call_cos(IID iid, IID argIID, HIGHPRECISION argv);
	void call_fabs(IID iid, IID argIID, HIGHPRECISION argv);
	void call_sqrt(IID iid, IID argIID, HIGHPRECISION argv);
	void call_log(IID iid, IID argIID, HIGHPRECISION argv);
	void call_floor(IID iid, IID argIID, HIGHPRECISION argv);
	void call_exp(IID iid, IID argIID, HIGHPRECISION argv);

	void fadd(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv);
	void fsub(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv);
	void fmul(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv);
	void fdiv(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv);

	void fload(IID iidV, double v, IID iid, void* vptr);
	void fstore(IID iidV, void* vptr);

	void fphi(IID out, double v, IID in);
	void fafter_call(IID iid, double v, IID return_id);

	void post_analysis();

private:
	const BlameShadowObject getShadowObject(IID iid, HIGHPRECISION v);

	const BlameShadowObject shadowFEval(IID iid, const BlameShadowObject& lBSO, const BlameShadowObject& rBSO, FBINOP op);

	const BlameShadowObject shadowFEval(IID iid, const BlameShadowObject& argBSO, MATHFUNC func);

	void computeBlameSummary(const BlameShadowObject& BSO, const BlameShadowObject& lBSO, const BlameShadowObject& rBSO,
							 FBINOP op);

	void computeBlameSummary(const BlameShadowObject& BSO, const BlameShadowObject& argBSO, MATHFUNC func);

	BlameNode computeBlameInformation(const BlameShadowObject& BSO, const BlameShadowObject& lBSO,
									  const BlameShadowObject& rBSO, FBINOP op, PRECISION p);

	BlameNode computeBlameInformation(const BlameShadowObject& BSO, const BlameShadowObject& argBSO, MATHFUNC func,
									  PRECISION p);

	bool canBlame(HIGHPRECISION result, HIGHPRECISION lop, HIGHPRECISION rop, FBINOP op, PRECISION p);

	bool isRequiredHigherPrecisionOperator(HIGHPRECISION result, HIGHPRECISION lop, HIGHPRECISION rop, FBINOP op,
										   PRECISION p);

	bool canBlame(HIGHPRECISION result, HIGHPRECISION arg, MATHFUNC func, PRECISION p);

	void fbinop(IID iid, IID liid, IID riid, HIGHPRECISION lv, HIGHPRECISION rv, FBINOP op);

	void call_lib(IID iid, IID argIID, HIGHPRECISION v, MATHFUNC func);

	void copyBlameSummary(IID dest, IID src);

	void copyShadowObject(IID dstIID, void* dstPtr, IID srcIID, void* srcPtr, double v);
};

#endif
