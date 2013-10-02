/**
 * @file AllocaInstrumenter.cpp
 * @brief
 */

#include "AllocaInstrumenter.h"

bool AllocaInstrumenter::CheckAndInstrument(Instruction* inst) {
  AllocaInst* allocaInst = dyn_cast<AllocaInst>(inst);

  if (allocaInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Constant* iidC = IID_CONSTANT(allocaInst);

    Constant* inxC = computeIndex(allocaInst);

    Type *type = allocaInst->getAllocatedType();
    if (!type) return false;
    KIND kind = TypeToKind(type);

    // if unsupported kind, return false
    if (kind == INV_KIND) return false;

    Constant* size;
    if (type->isArrayTy()) {
      ArrayType* aType = (ArrayType*) type;
      size = INT64_CONSTANT(aType->getNumElements(), UNSIGNED);
    } else {
      size = INT64_CONSTANT(0, UNSIGNED);
    }

    Constant* kindC = KIND_CONSTANT(kind);

    Instruction* call = CALL_IID_KIND_INT64_INT("llvm_allocax", iidC, kindC, size, inxC);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, allocaInst);

    return true;

  } else {
    return false;
  }
}
