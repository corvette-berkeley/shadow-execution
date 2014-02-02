
#include <math.h>
#include <glog/logging.h>

#include "NaNPropagationAnalysis.h"


void NaNPropagationAnalysis::load(IID iid UNUSED, KIND type, KVALUE* src, int file, int line, int inx) {
  InterpreterObserver::load(iid, type, src, file, line, inx);
  IValue* loadValue = executionStack.top()[inx];
  
  if (type == FLP32_KIND || type == FLP64_KIND || type == FLP128_KIND) {
    if (isnan(loadValue->getFlpValue())) {
      LOG(INFO) << file << ":" << line << ": WARNING Loading a NAN value " << endl;
    }
  }
  return;
}

void NaNPropagationAnalysis::store(IID iid UNUSED, KVALUE* dest, KVALUE* src, int file, int line, int inx) {
  InterpreterObserver::store(iid, dest, src, file, line, inx);

  // TODO: storing nan constant
  if (src->inx != -1) {
    IValue* srcValue = executionStack.top()[src->inx];
    
    if (srcValue->getType() == FLP32_KIND || srcValue->getType() == FLP64_KIND || srcValue->getType() == FLP128_KIND) {
      if (isnan(srcValue->getFlpValue())) {
	LOG(INFO) << file << ":" << line << ": WARNING Storing a NAN Value " << endl;
      }
    }
  }

  // case overwriting a NaN

  return;
}

