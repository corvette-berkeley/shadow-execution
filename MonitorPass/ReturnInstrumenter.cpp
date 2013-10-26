/**
 * @file ReturnInstrumenter.cpp
 * @brief
 */

#include "ReturnInstrumenter.h"

bool ReturnInstrumenter::CheckAndInstrument(Instruction* inst) {
  ReturnInst* returnInst = dyn_cast<ReturnInst>(inst);

  if (returnInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Constant* iidC = IID_CONSTANT(returnInst);

    Constant* inxC = computeIndex(returnInst);

    Value* retVal = returnInst->getReturnValue();

    if (retVal == NULL || retVal->getType()->isVoidTy()) {

      Instruction* call = CALL_IID_INT("llvm_return2_", iidC, inxC);
      instrs.push_back(call);

    } else if (retVal->getType()->isStructTy()) {

      KVALUE_STRUCTVALUE(retVal, instrs);

      Constant* valInxC = computeIndex(retVal);

      Instruction* call = CALL_IID_INT_INT("llvm_return_struct_", iidC, inxC, valInxC);
      instrs.push_back(call);
      
    } else {

      Value* ret = KVALUE_VALUE(retVal, instrs, NOSIGN);
      if (ret == NULL) return false;

      Instruction* call = CALL_IID_KVALUE_INT("llvm_return_", iidC, ret, inxC);
      instrs.push_back(call);

    }

    // instrument
    InsertAllBefore(instrs, returnInst);

    return true;

  } else {

    return false;

  }
}
