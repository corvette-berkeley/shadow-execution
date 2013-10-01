/**
 * @file CallInstrumenter.cpp
 * @brief
 */

#include "CallInstrumenter.h"

bool CallInstrumenter::CheckAndInstrument(Instruction* I) {
  CallInst* callInst = dyn_cast<CallInst>(I);

  if (callInst == NULL) {
    return false;
  } 

  // TODO: add support for instrinsic.
  if (dyn_cast<IntrinsicInst>(callInst) != NULL) {
    return false;
  }

  safe_assert(parent_ != NULL);

  count_++;

  InstrPtrVector instrs;

  Constant* iid = IID_CONSTANT(callInst);

  Constant* inx = computeIndex(callInst);

  // whether this call unwinds the stack
  bool noUnwind = callInst->getAttributes().hasAttrSomewhere(Attribute::NoUnwind);
  Constant* noUnwindC = BOOL_CONSTANT(noUnwind);

  // get return type
  Type* returnType = callInst->getType();
  KIND returnKind = TypeToKind(returnType);
  if(returnKind == INV_KIND) {
   return false; 
  }
  Constant* kind = KIND_CONSTANT(returnKind);

  // get pointer to the function
  Value* callValue = KVALUE_VALUE(callInst->getCalledValue(), instrs, NOSIGN);

  // get call arguments
  unsigned numArgs = callInst->getNumArgOperands();
  unsigned i;

  // push each arguments to the argument stack
  for (i = 0; i < numArgs; i++) {
    Value* arg = KVALUE_VALUE(callInst->getArgOperand(i), instrs, NOSIGN);
    Instruction* call = CALL_KVALUE("llvm_push_stack", arg);
    instrs.push_back(call);
  }

  Instruction* call = NULL;
  if (callInst->getCalledFunction()->getName() == "malloc") {
    call = CALL_IID_BOOL_KIND_KVALUE_INT("llvm_call_malloc", iid, noUnwindC, kind, callValue, inx);
  }
  else {
    call = CALL_IID_BOOL_KIND_KVALUE_INT("llvm_call", iid, noUnwindC, kind, callValue, inx);
  }
  instrs.push_back(call);

  // instrument
  InsertAllBefore(instrs, callInst);

  InstrPtrVector instrsAfter;
  if (noUnwind) {
    Value* callReturnValue = KVALUE_VALUE(callInst, instrsAfter, NOSIGN); 
    Instruction* call = CALL_KVALUE("llvm_call_nounwind", callReturnValue);
    instrsAfter.push_back(call);

    InsertAllAfter(instrsAfter, callInst);
  }

  return true;
}
