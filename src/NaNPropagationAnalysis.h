/**
 * @file NaNPropagationAnalysis.h
 * @brief NaNPropagation Declarations.
 */

#ifndef NAN_PROPAGATION_ANALYSIS_H_
#define NAN_PROPAGATION_ANALYSIS_H_

#include "InterpreterObserver.h"

class NaNPropagationAnalysis : public InterpreterObserver {

public:
	NaNPropagationAnalysis(std::string name) : InterpreterObserver(name) {}

	virtual void post_load(IID iid, KIND kind, SCOPE opScope, int opInx, uint64_t opAddr, bool loadGlobal, int loadInx,
						   int file, int line, int inx);

	virtual void post_store(int pInx, SCOPE pScope, KIND srcKind, SCOPE srcScope, int srcInx, int64_t srcValue, int file,
							int line, int inx);
};

#endif /* NAN_PROPAGATION_ANALYSIS_H_ */
