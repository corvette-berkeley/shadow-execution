#include "LoadInstrumenter.h"

bool LoadInstrumenter::CheckAndInstrument(Instruction *inst) {
  LoadInst* loadInst  = dyn_cast<LoadInst>(inst);

  if (loadInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    Constant* iid = IID_CONSTANT(loadInst);

    Constant* inx = computeIndex(loadInst);

    Type *type = loadInst->getType();
    if (!type) return false;
    while (type->isStructTy()) {
      StructType* structType = (StructType*) type;
      type = structType->getElementType(0);
    }
    
    KIND kind = TypeToKind(type);
    if (kind == INV_KIND) return false;
    Constant* kindC = KIND_CONSTANT(kind);

    InstrPtrVector instrs;
    Value* op = KVALUE_VALUE(loadInst->getPointerOperand(), instrs, NOSIGN);
    if(op == NULL) return false;

    Constant* CLine = INT32_CONSTANT(getLineNumber(loadInst), SIGNED);

    Instruction* call = CALL_IID_KIND_KVALUE_INT_INT("llvm_load", iid, kindC, op, CLine, inx);
    instrs.push_back(call);

    InsertAllBefore(instrs, loadInst);

    return true;

  } else {

    return false;

  }
}
