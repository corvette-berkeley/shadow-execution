#include "LoadInstrumenter.h"

bool LoadInstrumenter::CheckAndInstrument(Instruction *inst) {
  LoadInst* loadInst  = dyn_cast<LoadInst>(inst);

  if (loadInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    Constant* iid = IID_CONSTANT(loadInst);

    Constant* inx = computeIndex(loadInst);

    Constant* CLine = INT32_CONSTANT(getLineNumber(loadInst), SIGNED);

    InstrPtrVector instrs;

    Value* op = KVALUE_VALUE(loadInst->getPointerOperand(), instrs, NOSIGN);
    if(op == NULL) return false;

    Type *type = loadInst->getType();
    if (!type) return false;

    if (type->isStructTy()) {

      /*
       * Load for struct is treated separately.
       */

      KVALUE_STRUCTVALUE(loadInst, instrs);

      while (type->isStructTy()) {
        StructType* structType = (StructType*) type;
        type = structType->getElementType(0);
      }

      KIND kind = TypeToKind(type);
      if (kind == INV_KIND) return false;
      Constant* kindC = KIND_CONSTANT(kind);

      Instruction* call = CALL_IID_KIND_KVALUE_INT_INT("llvm_load_struct", iid, kindC, op, CLine, inx);
      instrs.push_back(call);

      InsertAllAfter(instrs, loadInst);

    } else {

      /*
       * Load for non-struct. 
       */

      KIND kind = TypeToKind(type);
      if (kind == INV_KIND) return false;
      Constant* kindC = KIND_CONSTANT(kind);

      Instruction* call = CALL_IID_KIND_KVALUE_INT_INT("llvm_load", iid, kindC, op, CLine, inx);
      instrs.push_back(call);

      InsertAllBefore(instrs, loadInst);

    }

    return true;

  } else {

    return false;

  }
}

uint64_t LoadInstrumenter::pushStructType(ArrayType* arrayType, InstrPtrVector& instrs) {
  Type* elemTy = arrayType;
  int size = 1;
  uint64_t allocation = 0;

  while (dyn_cast<ArrayType>(elemTy)) {
    size = size*((ArrayType*)elemTy)->getNumElements();
    elemTy = ((ArrayType*)elemTy)->getElementType();
  }

  KIND elemKind = TypeToKind(elemTy);
  safe_assert(elemKind != INV_KIND);

  if (elemKind == STRUCT_KIND) {
    for (int i = 0; i < size; i++) {
      allocation += pushStructType((StructType*)elemTy, instrs);
    }
  } else {
    Constant* elemKindC = KIND_CONSTANT(elemKind);

    for (int i = 0; i < size; i++) {
      Instruction* call = CALL_KIND("llvm_push_struct_type", elemKindC);
      instrs.push_back(call);
      allocation++;
    }
  }

  return allocation;
}

uint64_t LoadInstrumenter::pushStructType(StructType* structType, InstrPtrVector& instrs) {
  uint64_t size = structType->getNumElements();
  uint64_t allocation = 0;
  for (uint64_t i = 0; i < size; i++) {
    Type* elemType = structType->getElementType(i);
    KIND elemKind = TypeToKind(elemType);
    safe_assert(elemKind != INV_KIND);
    if (elemKind == ARRAY_KIND) {
      allocation += pushStructType((ArrayType*)elemType, instrs);
    } else if (elemKind == STRUCT_KIND) {
      allocation += pushStructType((StructType*)elemType, instrs);
    } else {
      Constant* elemKindC = KIND_CONSTANT(elemKind);
      Instruction* call = CALL_KIND("llvm_push_struct_type", elemKindC);
      instrs.push_back(call);
      allocation++;
    }
  }

  return allocation;
}
