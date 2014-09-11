#include "NaNPropagationAnalysis.h"
#include <cmath>
#include <glog/logging.h>
#include "IValue.h"

void NaNPropagationAnalysis::post_load(IID, KIND type, SCOPE, int, uint64_t, bool, int, int file, int line, int inx) {
	IValue* loadValue = executionStack.top()[inx];

	if (type == FLP32_KIND || type == FLP64_KIND || type == FLP128_KIND) {
		if (isnan(loadValue->getFlpValue())) {
			cout << file << ":" << line << ": WARNING Loading a NAN value " << endl;
		}
	}
	return;
}

void NaNPropagationAnalysis::post_store(int pInx, SCOPE, KIND, SCOPE, int, int64_t, int file, int line, int) {

	IValue* ptrResult = executionStack.top()[pInx];
	IValue* result = (IValue*)ptrResult->getIPtrValue();

	if (result->getType() == FLP32_KIND || result->getType() == FLP64_KIND || result->getType() == FLP128_KIND) {
		if (isnan(result->getFlpValue())) {
			cout << file << ":" << line << ": WARNING Storing a NAN Value " << endl;
		}
	}
	return;
}
