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

    Constant *cInx, *cScope, *cType, *cValue; // new

    if (retVal == NULL || retVal->getType()->isVoidTy()) {

      Instruction* call = CALL_IID_INT("llvm_return2_", iidC, inxC);
      instrs.push_back(call);

    } else if (retVal->getType()->isStructTy()) {

      KVALUE_STRUCTVALUE(retVal, instrs);

      Constant* valInxC = computeIndex(retVal);

      Instruction* call = CALL_IID_INT_INT("llvm_return_struct_", iidC, inxC, valInxC);
      instrs.push_back(call);
      
    } else {

      cInx = computeIndex(retVal);
      cScope = INT32_CONSTANT(getScope(retVal), SIGNED);
      cType = KIND_CONSTANT(TypeToKind(retVal->getType()));
      cValue = getValueOrIndex(retVal);

      Instruction* call = CALL_IID_INT_INT_KIND_INT64("llvm_return_", iidC, cInx, cScope, cType, cValue);
      instrs.push_back(call);

    }

    // instrument
    InsertAllBefore(instrs, returnInst);

    return true;

  } else {

    return false;

  }
}
