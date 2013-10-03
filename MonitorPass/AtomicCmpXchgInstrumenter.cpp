/**
 * @file AtomicCmpXchgInstrumenter.cpp
 * @brief
 */

#include "AtomicCmpXchgInstrumenter.h"

bool AtomicCmpXchgInstrumenter::CheckAndInstrument(Instruction* inst) {
  AtomicCmpXchgInst* cmpXchgInst = dyn_cast<AtomicCmpXchgInst>(inst); 

  if (cmpXchgInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;
    Value* cmpOp = KVALUE_VALUE(cmpXchgInst->getCompareOperand(), instrs, NOSIGN);
    if(cmpOp == NULL) return false;

    Value* valOp = KVALUE_VALUE(cmpXchgInst->getNewValOperand(), instrs, NOSIGN);
    if(valOp == NULL) return false;

    Constant* iidC = IID_CONSTANT(cmpXchgInst);

    Constant* inxC = computeIndex(cmpXchgInst);

    Instruction* I_cast_ptr = PTR_CAST_INSTR(cmpXchgInst->getPointerOperand());
    instrs.push_back(I_cast_ptr);

    Instruction* call = CALL_IID_PTR_KVALUE_KVALUE_INT("llvm_cmpxchg", iidC, I_cast_ptr, cmpOp, valOp, inxC);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, inst);

    return true;

  } else {

    return false;

  }
}
