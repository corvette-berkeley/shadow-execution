/**
 * @file IndirectBrInstrumenter.cpp
 * @brief
 */

#include "IndirectBrInstrumenter.h"

bool IndirectBrInstrumenter::CheckAndInstrument(Instruction* inst) {
  IndirectBrInst* inbrInst = dyn_cast<IndirectBrInst>(inst);

  if (inbrInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Constant* iidC = IID_CONSTANT(inbrInst);

    Constant* inxC = computeIndex(inbrInst);

    Value* addr = KVALUE_VALUE(inbrInst->getAddress(), instrs, NOSIGN);
    if (addr == NULL) return false;

    Instruction* call = CALL_IID_KVALUE_INT("llvm_indirectbr",
        iidC, addr, inxC);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, inbrInst);

    return true;

  } else {

    return false;

  }
}
