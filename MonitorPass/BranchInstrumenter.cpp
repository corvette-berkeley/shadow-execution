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

    Constant* inxC = computeIndex(branchInst);

    //Constant *cInx, *cScope, *cType, *cValue; // new
    Constant *cInx, *cScope, *cType; // new

    if (branchInst->isConditional()) {

      //Value* condition = KVALUE_VALUE(branchInst->getCondition(), instrs, NOSIGN);
      //if(condition == NULL) return false;

      Value *condition = branchInst->getCondition();
      cInx = computeIndex(condition);
      cScope = INT32_CONSTANT(getScope(condition), NOSIGN); // or SIGNED?
      cType = KIND_CONSTANT(TypeToKind(condition->getType()));
      //cValue = getValueOrIndex(condition);

      Value *ptrOp = condition;
      //Instruction *ptrOpCast = PTRTOINT_CAST_INSTR(ptrOp);
      Instruction *ptrOpCast = CAST_VALUE(ptrOp, NOSIGN);
      if (!ptrOpCast) return NULL;
      instrs.push_back(ptrOpCast);

      //Instruction* call = CALL_IID_BOOL_KVALUE_INT("llvm_branch", iidC, isConditionalC, condition, inxC);
      //Instruction* call = CALL_IID_BOOL_INT_INT_KIND_INT64_INT("llvm_branch", iidC, isConditionalC, cInx, cScope, cType, cValue, inxC);
      Instruction* call = CALL_IID_BOOL_INT_INT_KIND_INT64_INT("llvm_branch", iidC, isConditionalC, cInx, cScope, cType, ptrOpCast, inxC);

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
