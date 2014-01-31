#include "NaNPropagationAnalysis.h"

void NaNPropagationAnalysis::load(IID iid UNUSED, KIND type, KVALUE* src, int file, int line, int inx) {
  InterpreterObserver::load(iid, type, src, file, line, inx);
  IValue* loadedValue = executionStack.top()[inx];
  cout << "[NaNPropagation] " << loadedValue->toString() << endl;
  return;
}

void NaNPropagationAnalysis::store(IID iid UNUSED, KVALUE* dest, KVALUE* src, int file, int line, int inx) {
  InterpreterObserver::store(iid, dest, src, file, line, inx);
  IValue* destValue = executionStack.top()[dest->inx];
  cout << "[NaNPropagation] " << destValue->toString() << endl;    
  return;
}

