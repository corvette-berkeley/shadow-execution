/**
 * @file GetElementPtrInstrumenter.cpp
 * @brief
 */

#include "GetElementPtrInstrumenter.h"

bool GetElementPtrInstrumenter::CheckAndInstrument(Instruction* inst) {
  GetElementPtrInst* gepInst = dyn_cast<GetElementPtrInst>(inst);

  if (gepInst == NULL) {
    return false;
  }

  safe_assert(parent_ != NULL);

  count_++;

  InstrPtrVector instrs;

  Constant* iidC = IID_CONSTANT(gepInst);

  Constant* inxC = computeIndex(gepInst);

  Constant* inbound = BOOL_CONSTANT(gepInst->isInBounds());

  Value* ptrOp = KVALUE_VALUE(gepInst->getPointerOperand(), instrs, NOSIGN);
  if(ptrOp == NULL) return false;

  // new
  Constant* idxOp = INT64_CONSTANT(gepInst->getPointerOperandIndex(), UNSIGNED);

  PointerType* T = (PointerType*) gepInst->getPointerOperandType();
  Type* elemT = T->getElementType();
  KIND kind = TypeToKind(elemT);

  Constant* kindC = KIND_CONSTANT(kind);

  Constant* size;
  if (elemT->isArrayTy()) {
    ArrayType* aType = (ArrayType*) T;
    size = INT64_CONSTANT(aType->getNumElements(), UNSIGNED);
  } else {
    size = INT64_CONSTANT(0, UNSIGNED);
  }

  Instruction* call = CALL_IID_BOOL_KVALUE_KIND_INT64_INT64_INT("llvm_getelementptr", iidC, inbound, ptrOp, kindC, size, idxOp, inxC);
  instrs.push_back(call);

  // instrument
  InsertAllBefore(instrs, gepInst);

  return true;
}
