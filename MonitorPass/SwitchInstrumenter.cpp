/**
 * @file SwitchInstrumenter.cpp
 * @brief
 */

#include "SwitchInstrumenter.h"

bool SwitchInstrumenter::CheckAndInstrument(Instruction* inst) {
  SwitchInst* switchInst = dyn_cast<SwitchInst>(inst);

  if (switchInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Constant* iidC = IID_CONSTANT(switchInst);

    Constant* inxC = computeIndex(switchInst);

    Value* condition = switchInst->getCondition();

    Instruction* call = CALL_IID_KVALUE_INT("llvm_switch_", iidC, condition, inxC);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, switchInst);

    return true;

  } else {

    return false;

  }
}
