
#include <math.h>
#include <glog/logging.h>

#include "NaNPropagationAnalysis.h"

void NaNPropagationAnalysis::post_load(IID iid UNUSED, KIND type UNUSED, SCOPE opScope UNUSED, int opInx UNUSED, uint64_t opAddr UNUSED, bool loadGlobal UNUSED, 
				       int loadInx UNUSED, int file, int line, int inx) {
  IValue* loadValue = executionStack.top()[inx];

  if (type == FLP32_KIND || type == FLP64_KIND || type == FLP128_KIND) {
    if (isnan(loadValue->getFlpValue())) {
      cout << file << ":" << line << ": WARNING Loading a NAN value " << endl;
    }
  }
  return;
}

void NaNPropagationAnalysis::post_store(int pInx, SCOPE pScope UNUSED, KIND srcKind UNUSED, SCOPE srcScope UNUSED, int srcInx UNUSED, int64_t srcValue UNUSED,
					int file, int line, int inx UNUSED) {

  IValue* ptrResult = executionStack.top()[pInx];
  IValue* result = (IValue*)ptrResult->getIPtrValue();
  
  if (result->getType() == FLP32_KIND || result->getType() == FLP64_KIND || result->getType() == FLP128_KIND) {
    if (isnan(result->getFlpValue())) {
      cout << file << ":" << line << ": WARNING Storing a NAN Value " << endl;
    }
  }
  return;
}
