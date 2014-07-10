/**
 * @file BranchInstrumenter.cpp
 * @brief
 */

#include "BranchInstrumenter.h"

bool BranchInstrumenter::CheckAndInstrument(Instruction* inst) {
  BranchInst* branchInst = dyn_cast<BranchInst>(inst);

  if (branchInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Constant* isConditionalC = BOOL_CONSTANT(branchInst->isConditional());

    Constant* iidC = IID_CONSTANT(branchInst);

    Constant *cInx, *cScope, *cType; // new

    if (branchInst->isConditional()) {

      Value *condition = branchInst->getCondition();
      cInx = computeIndex(condition);
      cScope = INT32_CONSTANT(getScope(condition), NOSIGN); // or SIGNED?
      cType = KIND_CONSTANT(TypeToKind(condition->getType()));

      Value *ptrOp = condition;
      Instruction *ptrOpCast = CAST_VALUE(ptrOp, instrs, NOSIGN);

      if (!ptrOpCast) return NULL;
      instrs.push_back(ptrOpCast);

      Instruction* call = CALL_IID_BOOL_INT_INT_KIND_INT64("llvm_branch", iidC, isConditionalC, cInx, cScope, cType, ptrOpCast);
      instrs.push_back(call);
    } 
    else {
      Instruction* call = CALL_IID_BOOL("llvm_branch2", iidC, isConditionalC);
      instrs.push_back(call);
    }

    // instrument
    InsertAllBefore(instrs, branchInst);
    return true;
  } 
  else {
    return false;
  }
}
