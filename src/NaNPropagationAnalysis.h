/**
 * @file NaNPropagationAnalysis.h
 * @brief NaNPropagation Declarations.
 */

#ifndef NAN_PROPAGATION_ANALYSIS_H_
#define NAN_PROPAGATION_ANALYSIS_H_

#include "InterpreterObserver.h"
#include "IValue.h"

using namespace std;
using namespace llvm;

class NaNPropagationAnalysis : public InterpreterObserver {

 public:
  NaNPropagationAnalysis(std::string name) : InterpreterObserver(name) {}
  
  virtual void load(IID iid, KIND kind, KVALUE* op, bool loadGlobal, int loadInx, int file, int line, int inx);
  
  virtual void store(int pInx, SCOPE pScope, KVALUE *op, int file, int line, int inx);

 private:
  //bool isOutOfBound(IValue* pointer);
};

#endif /* NAN_PROPAGATION_ANALYSIS_H_ */
