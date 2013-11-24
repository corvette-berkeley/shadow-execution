/**
 * @file SelectInstrumenter.cpp
 * @brief
 */

#include "SelectInstrumenter.h"

bool SelectInstrumenter::CheckAndInstrument(Instruction* inst) {
  SelectInst* selectInst = dyn_cast<SelectInst>(inst);

  if (selectInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Constant* iidC = IID_CONSTANT(selectInst);

    Constant* inxC = computeIndex(selectInst);

    Value* condition = KVALUE_VALUE(selectInst->getCondition(), instrs, NOSIGN);
    if(condition == NULL) return false;

    Value* tvalue = KVALUE_VALUE(selectInst->getTrueValue(), instrs, NOSIGN);
    if(tvalue == NULL) return false;
    
    Value* fvalue = KVALUE_VALUE(selectInst->getFalseValue(), instrs, NOSIGN);
    if(fvalue == NULL) return false;
    
    Instruction* call = CALL_IID_KVALUE_KVALUE_KVALUE_INT("llvm_select", iidC, condition, tvalue, fvalue, inxC);
    
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, selectInst);

    return true;
  } 

  return false;
}
