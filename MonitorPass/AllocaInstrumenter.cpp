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

    // alloca for argument?
    bool isArgument = false;
    Value::use_iterator it = allocaInst->use_begin();
    if (StoreInst *store = dyn_cast<StoreInst>(*it)) {
      if (dyn_cast<Argument>(store->getValueOperand())) {
	isArgument = true;
      }
    }
	
    Constant* isArgumentC = BOOL_CONSTANT(isArgument);
    

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

      if (elemKind == STRUCT_KIND) pushStructType((StructType*) elemType, instrs);

      Constant* elemKindC = KIND_CONSTANT(elemKind);

      Instruction* call = CALL_IID_KIND_INT64_INT_BOOL("llvm_allocax_array", iidC, elemKindC, INT64_CONSTANT(0, UNSIGNED), inxC, isArgumentC);
      instrs.push_back(call);

    } else if (type->isStructTy()) {
      // alloca for struct type
      StructType* structType = (StructType*) type;
      uint64_t size = pushStructType(structType, instrs);

      Constant* sizeC = INT64_CONSTANT(size, UNSIGNED);
      
      Instruction* call = CALL_IID_INT64_INT_BOOL("llvm_allocax_struct", iidC, sizeC, inxC, isArgumentC);
        instrs.push_back(call);
    } else {
      // alloca for scalar and pointer type
      
      Constant* size;
      size = INT64_CONSTANT(0, UNSIGNED);
      Instruction* call = CALL_IID_KIND_INT64_INT_BOOL("llvm_allocax", iidC, kindC, size, inxC, isArgumentC);
      instrs.push_back(call);
    }

    // instrument
    InsertAllBefore(instrs, allocaInst);

    return true;

  } else {
    return false;
  }
}

uint64_t AllocaInstrumenter::pushStructType(StructType* structType, InstrPtrVector& instrs) {
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

uint64_t AllocaInstrumenter::pushStructType(ArrayType* arrayType, InstrPtrVector& instrs) {
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

