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
    if (kind == INV_KIND) return false;
    Constant* kindC = KIND_CONSTANT(kind);


    if (type->isArrayTy()) {
      // alloca for array type
      
      Type* elemType = type;
      while (dyn_cast<ArrayType>(elemType)) {
        Constant* size = INT64_CONSTANT(((ArrayType*)elemType)->getNumElements(), UNSIGNED);
        Instruction* call = CALL_INT64("llvm_push_array_size", size);
        instrs.push_back(call);
        elemType = ((ArrayType*)elemType)->getElementType();
      }

      KIND elemKind = TypeToKind(elemType);
      if (elemKind == INV_KIND) return false;
      Constant* elemKindC = KIND_CONSTANT(elemKind);

      Instruction* call = CALL_IID_KIND_INT64_INT("llvm_allocax_array", iidC, elemKindC, INT64_CONSTANT(0, UNSIGNED), inxC);
      instrs.push_back(call);

    } else if (type->isStructTy()) {
      // alloca for struct type
      StructType* structType = (StructType*) type;
      uint64_t size = structType->getNumElements();
      for (uint64_t i = 0; i < size; i++) {
        Type* elemType = structType->getElementType(i);
        KIND elemKind = TypeToKind(elemType);
        // debug information
        if (elemKind == INV_KIND) {
          printf("[AllocaIstrumenter => Unknown type of struct element!\n");
          abort();
        }
        Constant* elemKindC = KIND_CONSTANT(elemKind);
        Instruction* call = CALL_KIND("llvm_push_struct_type", elemKindC);
        instrs.push_back(call);
      }

      Constant* sizeC = INT64_CONSTANT(size, UNSIGNED);
      
      Instruction* call = CALL_IID_INT64_INT("llvm_allocax_struct", iidC, sizeC, inxC);
        instrs.push_back(call);
    } else {
      // alloca for scalar and pointer type
      
      Constant* size;
      size = INT64_CONSTANT(0, UNSIGNED);
      Instruction* call = CALL_IID_KIND_INT64_INT("llvm_allocax", iidC, kindC, size, inxC);
      instrs.push_back(call);
    }

    // instrument
    InsertAllBefore(instrs, allocaInst);

    return true;

  } else {
    return false;
  }
}
