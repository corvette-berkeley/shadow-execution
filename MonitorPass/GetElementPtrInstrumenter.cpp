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

    Instruction* call = CALL_IID_BOOL_KVALUE_KIND_INT("llvm_getelementptr_array", iidC, inbound, ptrOp, kindC, inxC);

    instrs.push_back(call);

  } else if (elemT->isStructTy()) {
    // this branch is the case for local struct
    
    for (User::op_iterator idx = gepInst->idx_begin(); idx != gepInst->idx_end(); idx++) {
      Value* idxOp = KVALUE_VALUE(idx->get(), instrs, NOSIGN);
      Instruction* call = CALL_KVALUE("llvm_push_getelementptr_inx", idxOp);
      instrs.push_back(call);
    } 


    Type* gepInstType = ((PointerType*) gepInst->getType())->getElementType();
    KIND kind = TypeToKind(gepInstType);
    Constant* kindC = KIND_CONSTANT(kind);

    Type* elemArrayType = gepInstType; // used only when element is array type

    /*
    if (gepInstType->isArrayTy()) {

      Type* elemType = gepInstType;
      while (dyn_cast<ArrayType>(elemType)) {
        Constant* size = INT64_CONSTANT(((ArrayType*)elemType)->getNumElements(), UNSIGNED);
        Instruction* call = CALL_INT64("llvm_push_array_size", size);
        instrs.push_back(call);
        elemType = ((ArrayType*)elemType)->getElementType();
      }

      elemArrayType = elemType;

    } else if (gepInstType->isStructTy()) {

      StructType* structType = (StructType*) gepInstType;
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

    }
    */

    KIND elemArrayKind = TypeToKind(elemArrayType);
    Constant* elemArrayKindC = KIND_CONSTANT(elemArrayKind);

    Instruction* call = CALL_IID_BOOL_KVALUE_KIND_KIND_INT("llvm_getelementptr_struct", iidC, inbound, ptrOp, kindC, elemArrayKindC, inxC);

    instrs.push_back(call);

  } else {
    // this branch is the case for heap

    if (gepInst->getNumIndices() != 1) {
      cout << "[GetElementPtr] => Multiple indices\n";
      abort();
    }

    Value* idxOp = KVALUE_VALUE(gepInst->idx_begin()->get(), instrs, NOSIGN);
    if(idxOp == NULL) return false;  

    KIND kind = TypeToKind(elemT);

    Constant* kindC = KIND_CONSTANT(kind);

    Constant* size = INT64_CONSTANT(elemT->getPrimitiveSizeInBits(), false);

    Instruction* call = CALL_IID_BOOL_KVALUE_KVALUE_KIND_INT64_INT("llvm_getelementptr", iidC, inbound, ptrOp, idxOp, kindC, size, inxC);
    instrs.push_back(call);

  }

  // instrument
  InsertAllBefore(instrs, gepInst);

  return true;
}
