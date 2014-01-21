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

  PointerType* T = (PointerType*) gepInst->getPointerOperandType();
  Type* elemT = T->getElementType();

  if (elemT->isArrayTy()) {
    // this branch is the case for local array
    
    for (User::op_iterator idx = gepInst->idx_begin(); idx != gepInst->idx_end(); idx++) {
      Value* idxOp = KVALUE_VALUE(idx->get(), instrs, NOSIGN);
      Instruction* call = CALL_KVALUE("llvm_push_getelementptr_inx", idxOp);
      instrs.push_back(call);
    } 

    while (dyn_cast<ArrayType>(elemT)) {
      Constant* size = INT64_CONSTANT(((ArrayType*)elemT)->getNumElements(), UNSIGNED);
      Instruction* call = CALL_INT64("llvm_push_array_size", size);
      instrs.push_back(call);
      elemT = ((ArrayType*)elemT)->getElementType();
    }

    Type* gepInstType = ((PointerType*) gepInst->getType())->getElementType();
    KIND kind = TypeToKind(gepInstType);
    Constant* kindC = KIND_CONSTANT(kind);
     
    if (kind == STRUCT_KIND) {
      pushStructElementSize((StructType*) elemT, instrs);
    }

    Instruction* call = CALL_IID_BOOL_KVALUE_KIND_INT("llvm_getelementptr_array", iidC, inbound, ptrOp, kindC, inxC);

    instrs.push_back(call);

  } else if (elemT->isStructTy()) {
    // this branch is the case for local struct
    StructType* structType = (StructType*) elemT;
    pushStructType(structType, instrs);
    
    for (User::op_iterator idx = gepInst->idx_begin(); idx != gepInst->idx_end(); idx++) {
      Value* idxOp = KVALUE_VALUE(idx->get(), instrs, NOSIGN);
      Instruction* call = CALL_KVALUE("llvm_push_getelementptr_inx", idxOp);
      instrs.push_back(call);
    } 

    // size of struct elements
    pushStructElementSize((StructType*) elemT, instrs);

    Type* gepInstType = ((PointerType*) gepInst->getType())->getElementType();
    KIND kind = TypeToKind(gepInstType);
    Constant* kindC = KIND_CONSTANT(kind);

    Type* elemArrayType = gepInstType; // used only when element is array type

    KIND elemArrayKind = TypeToKind(elemArrayType);
    Constant* elemArrayKindC = KIND_CONSTANT(elemArrayKind);

    Instruction* call = CALL_IID_BOOL_KVALUE_KIND_KIND_INT("llvm_getelementptr_struct", iidC, inbound, ptrOp, kindC, elemArrayKindC, inxC);

    instrs.push_back(call);

  } else {
    // this branch is the case for heap

    if (gepInst->getNumIndices() != 1) {
      cout << "[GetElementPtr] => Multiple indices" << endl;
      abort();
    }

    Value* idxOp = KVALUE_VALUE(gepInst->idx_begin()->get(), instrs, NOSIGN);
    if(idxOp == NULL) return false;  

    KIND kind = TypeToKind(elemT);

    Constant* kindC = KIND_CONSTANT(kind);

    Constant* size;
    int isize = 0;

    if (elemT->isPointerTy()) {
      isize = 64;
      size = INT64_CONSTANT(isize, false);
    }
    else {
      isize = elemT->getPrimitiveSizeInBits();
      size = INT64_CONSTANT(isize, false);
    }
    assert(isize != 0);

    Constant* line = INT32_CONSTANT(getLineNumber(gepInst), SIGNED);

    Instruction* call = CALL_IID_BOOL_KVALUE_KVALUE_KIND_INT64_INT_INT("llvm_getelementptr", iidC, inbound, ptrOp, idxOp, kindC, size, line, inxC);
    instrs.push_back(call);

  }

  // instrument
  InsertAllBefore(instrs, gepInst);

  return true;
}

void GetElementPtrInstrumenter::pushStructElementSize(StructType* structType, InstrPtrVector& instrs) {
  uint64_t size, i, allocation;

  size = structType->getNumElements();
  for (i = 0; i < size; i++) {
    Type* elemType = structType->getElementType(i);
    KIND elemKind = TypeToKind(elemType);
    safe_assert(elemKind != INV_KIND);
    if (elemKind == ARRAY_KIND) {
      allocation = getFlatSize((ArrayType*) elemType);
    } else if (elemKind == STRUCT_KIND) {
      allocation = getFlatSize((StructType*) elemType);
    } else {
      allocation = 1;
    }
    Constant* allocC = INT64_CONSTANT(allocation, NOSIGN);
    Instruction* call = CALL_INT64("llvm_push_struct_element_size", allocC);
    instrs.push_back(call);
  }
}

uint64_t GetElementPtrInstrumenter::getFlatSize(ArrayType* arrayType) {
  Type* elemTy; 
  int size; 
  uint64_t allocation;
  KIND elemKind;

  // 
  // for multi-dimensional array
  // get flatten size
  //
  size = 1;
  elemTy = arrayType;
  while (dyn_cast<ArrayType>(elemTy)) {
    size = size*((ArrayType*)elemTy)->getNumElements();
    elemTy = ((ArrayType*)elemTy)->getElementType();
  }

  //
  // get element kind
  //
  elemKind = TypeToKind(elemTy);
  safe_assert(elemKind != INV_KIND);

  //
  // element kind can be struct or primitive type
  //
  if (elemKind == STRUCT_KIND) {
    allocation = size* getFlatSize( (StructType*) elemTy );
  } else {
    allocation = size; 
  }

  return allocation;
}

uint64_t GetElementPtrInstrumenter::getFlatSize(StructType* structType) {
  uint64_t size, allocation, i;

  allocation = 0;
  size = structType->getNumElements();

  //
  // iterate through all struct element types
  // accumulate allocation size
  //
  for (i = 0; i < size; i++) {
    Type* elemType = structType->getElementType(i);
    KIND elemKind = TypeToKind(elemType);
    safe_assert(elemKind != INV_KIND);

    if (elemKind == ARRAY_KIND) {
      allocation += getFlatSize((ArrayType*) elemType);
    } else if (elemKind == STRUCT_KIND) {
      allocation += getFlatSize((StructType*) elemType);
    } else {
      allocation += 1;
    }
  }

  return allocation;
}

uint64_t GetElementPtrInstrumenter::pushStructType(ArrayType* arrayType, InstrPtrVector& instrs) {
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

uint64_t GetElementPtrInstrumenter::pushStructType(StructType* structType, InstrPtrVector& instrs) {
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
