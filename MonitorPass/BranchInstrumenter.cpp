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

    Constant* isConditionalC =
      BOOL_CONSTANT(branchInst->isConditional());

    Constant* iidC = IID_CONSTANT(branchInst);

    Constant* inxC = computeIndex(branchInst);

    if (branchInst->isConditional()) {

      Value* condition = KVALUE_VALUE(branchInst->getCondition(),
          instrs, NOSIGN);
      if(condition == NULL) return false;

      Instruction* call = CALL_IID_BOOL_KVALUE_INT("llvm_branch",
          iidC, isConditionalC, condition, inxC);

      instrs.push_back(call);

    } else {

      Instruction* call = CALL_IID_BOOL_INT("llvm_branch2", iidC, isConditionalC, inxC);

      instrs.push_back(call);

    }

    // instrument
    InsertAllBefore(instrs, branchInst);

    return true;

  } else {

    return false;

  }
}
