/**
 * @file ExtractValueInstrumenter.cpp
 * @brief
 */

#include "ExtractValueInstrumenter.h"

bool ExtractValueInstrumenter::CheckAndInstrument(Instruction* inst) { 
  ExtractValueInst* evInst = dyn_cast<ExtractValueInst>(inst); 

  if (evInst == NULL) {
    return false;
  }

  safe_assert(parent_ != NULL);

  count_++;

  InstrPtrVector instrs;

  Constant* iidC = IID_CONSTANT(evInst);

  Constant* inxC = computeIndex(evInst);

  Value* aggOp = evInst->getAggregateOperand();

  Constant* aggOpInxC = computeIndex(aggOp);

  KVALUE_STRUCTVALUE(aggOp, instrs);

  for (ExtractValueInst::idx_iterator idx = evInst->idx_begin(); idx != evInst->idx_end(); idx++) {
    Constant* idxOp = INT32_CONSTANT(*idx, UNSIGNED);
    Instruction* call = CALL_INT("llvm_push_getelementptr_inx2", idxOp);
    instrs.push_back(call);
  } 

  Instruction* call = CALL_IID_INT_INT("llvm_extractvalue", iidC, inxC, aggOpInxC);

  instrs.push_back(call);

  InsertAllBefore(instrs, evInst);

  return true;
}
