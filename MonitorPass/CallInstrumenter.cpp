/**
 * @file CallInstrumenter.cpp
 * @brief
 */

#include "CallInstrumenter.h"
#include "../src/IValue.h"

bool CallInstrumenter::CheckAndInstrument(Instruction* I) {
  CallInst* callInst = dyn_cast<CallInst>(I);

  if (callInst == NULL) {
    return false;
  } 

  bool isIntrinsic = dyn_cast<IntrinsicInst>(callInst) != NULL;
  bool noUnwind = callInst->getAttributes().hasAttrSomewhere(Attribute::NoUnwind) || isIntrinsic;

  if (noUnwind && callInst->getType()->isVoidTy()) {
    return false;
  }
  
  safe_assert(parent_ != NULL);

  count_++;

  InstrPtrVector instrs;

  Constant* iid = IID_CONSTANT(callInst);
  Constant* inx = computeIndex(callInst);

  // for later reference: iid and function name
  Function *callee = callInst->getCalledFunction();
  if (callee) {
    cout << "called function: " << callee->getName().str() << endl;
    iid->dump();
  }

  // whether this call unwinds the stack
  if (callee != NULL && callee->getName() == "malloc") {
    noUnwind = false;
  }
  Constant* noUnwindC = BOOL_CONSTANT(noUnwind);

  // get return type
  Type* returnType = callInst->getType();
  KIND returnKind = TypeToKind(returnType);
  if(returnKind == INV_KIND) {
    cout << "[CallInstrumenter] => Return type == INV_KIND" << endl;
    safe_assert(false);
  }
  Constant* kind = KIND_CONSTANT(returnKind);

  // get call arguments
  unsigned numArgs = noUnwind ? 0 : callInst->getNumArgOperands();
  unsigned i;

  // push each arguments to the argument stack
  for (i = 0; i < numArgs; i++) {
    Value* arg = KVALUE_VALUE(callInst->getArgOperand(i), instrs, NOSIGN);
    Instruction* call = CALL_KVALUE("llvm_push_stack", arg);
    instrs.push_back(call);
  }

  Instruction* call = NULL;

  if (callInst->getCalledFunction() != NULL &&
      callInst->getCalledFunction()->getName() == "malloc") {
    Value* callValue = KVALUE_VALUE(callInst->getCalledValue(), instrs, NOSIGN); 


    // there can be more than 1 use
    // e.g., checking for NULL
    // assume bitcast to same type if more than one
    BitCastInst *bitcast = NULL;
    Value::use_iterator it = callInst->use_begin();
    for(; it != callInst->use_end(); it++) {
      if ((bitcast = dyn_cast<BitCastInst>(*it)) != NULL) {
	break;
      }
    }

    Constant* size = NULL;
    if (bitcast != NULL) {
      bitcast->dump();

      //PointerType *src = dyn_cast<PointerType>(bitcast->getSrcTy());
      PointerType *dest = dyn_cast<PointerType>(bitcast->getDestTy());

      //cout << "Sizes: " << src->getElementType()->getPrimitiveSizeInBits() << " " << 
      //  dest->getElementType()->getPrimitiveSizeInBits() << endl;

      //cout << "Number of bytes requested: " << endl;
      callInst->getOperand(0)->dump();      

      returnKind = TypeToKind(dest->getElementType());
      if(returnKind == INV_KIND) {
        return false; 
      }
      kind = KIND_CONSTANT(returnKind);

      if (TypeToKind(dest->getElementType()) == STRUCT_KIND) {
	uint64_t allocation = pushStructType((StructType*) dest->getElementType(), instrs);
	cout << "After allocation: " << allocation << endl;
	/*
        if (StructType *st = dyn_cast<StructType>(dest->getElementType())) {
          unsigned numElems = st->getNumElements();
          unsigned sum = 0;
          for(unsigned i = 0; i < numElems; i++) {
            Type* type = st->getElementType(i);
            sum =+ type->getPrimitiveSizeInBits();
          }
        }
	*/
      }
      size = INT32_CONSTANT(dest->getElementType()->getPrimitiveSizeInBits(), false);
    }
    else {
      returnKind = INT8_KIND;
      kind = KIND_CONSTANT(returnKind);

      size = INT32_CONSTANT(8, false);
    }

    // kind is the type of each element (the return type is known to be PTR)
    noUnwind = false;
    call = CALL_IID_BOOL_KIND_KVALUE_INT_INT("llvm_call_malloc", iid, noUnwindC, kind, callValue, size, inx);
  }
  else {
    // kind is the return type of the function
    call = CALL_IID_BOOL_KIND_INT("llvm_call", iid, noUnwindC, kind, inx);
  }
  instrs.push_back(call);

  // instrument
  InsertAllBefore(instrs, callInst);

  InstrPtrVector instrsAfter;
  if (callInst->getCalledFunction() == NULL || callInst->getCalledFunction()->getName() != "malloc") {
    Instruction* call = NULL;

    if (returnType->isVoidTy()) {
      call = CALL("llvm_after_void_call");

    } else if (returnType->isStructTy()){
      KVALUE_STRUCTVALUE(callInst, instrsAfter);
      call = CALL("llvm_after_struct_call");

    } else {
      Value* callReturnValue = KVALUE_VALUE(callInst, instrsAfter, SIGNED); 
      call = CALL_KVALUE("llvm_after_call", callReturnValue);

    }
    instrsAfter.push_back(call);
    InsertAllAfter(instrsAfter, callInst);

  }
  return true;
}


uint64_t CallInstrumenter::pushStructType(StructType* structType, InstrPtrVector& instrs) {
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


uint64_t CallInstrumenter::pushStructType(ArrayType* arrayType, InstrPtrVector& instrs) {
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
