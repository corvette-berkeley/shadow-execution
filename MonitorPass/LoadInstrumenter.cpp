#include "LoadInstrumenter.h"

bool LoadInstrumenter::CheckAndInstrument(Instruction *inst) {
  LoadInst* loadInst  = dyn_cast<LoadInst>(inst);

  if (loadInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    Constant* iid = IID_CONSTANT(loadInst);

    Constant* inx = computeIndex(loadInst);

    InstrPtrVector instrs;
    Value* op = KVALUE_VALUE(loadInst->getPointerOperand(), instrs, NOSIGN);
    if(op == NULL) return false;

    Instruction* call = CALL_IID_KVALUE_INT("llvm_load", iid, op, inx);
    instrs.push_back(call);

    InsertAllBefore(instrs, loadInst);

    return true;

  } else {

    return false;

  }
}
