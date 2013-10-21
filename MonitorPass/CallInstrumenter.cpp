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

  // TODO: add support for instrinsic.
  if (dyn_cast<IntrinsicInst>(callInst) != NULL && callInst->getType()->isVoidTy()) {
    return false;
  }
  
  safe_assert(parent_ != NULL);

  count_++;

  InstrPtrVector instrs;

  Constant* iid = IID_CONSTANT(callInst);

  Constant* inx = computeIndex(callInst);

  // whether this call unwinds the stack
  bool noUnwind = callInst->getAttributes().hasAttrSomewhere(Attribute::NoUnwind) || isIntrinsic;
  Constant* noUnwindC = BOOL_CONSTANT(noUnwind);

  // get return type
  Type* returnType = callInst->getType();
  KIND returnKind = TypeToKind(returnType);
  if(returnKind == INV_KIND) {
    cout << "[CallInstrumenter] => Return type == INV_KIND" << endl;
    safe_assert(false);
  }
  Constant* kind = KIND_CONSTANT(returnKind);

  // get pointer to the function
  // Value* callValue = noUnwind ? KVALUE_VALUE(callInst, instrs, NOSIGN) :
  //  KVALUE_VALUE(callInst->getCalledValue(), instrs, NOSIGN);

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

  if (callInst->getCalledFunction()->getName() == "malloc") {
    Value* callValue = KVALUE_VALUE(callInst->getCalledValue(), instrs, NOSIGN); 

    safe_assert(callInst->getNumUses() == 1);

    Constant* size = NULL;
    if (BitCastInst *bitcast = dyn_cast<BitCastInst>(*callInst->use_begin())) {
      bitcast->dump();

      PointerType *src = dyn_cast<PointerType>(bitcast->getSrcTy());
      PointerType *dest = dyn_cast<PointerType>(bitcast->getDestTy());

      cout << "Sizes: " << src->getElementType()->getPrimitiveSizeInBits() << " " << 
        dest->getElementType()->getPrimitiveSizeInBits() << endl;

      cout << "Number of bytes requested: " << endl;
      callInst->getOperand(0)->dump();      

      returnKind = TypeToKind(dest->getElementType());
      if(returnKind == INV_KIND) {
        return false; 
      }
      kind = KIND_CONSTANT(returnKind);

      if (TypeToKind(dest->getElementType()) == STRUCT_KIND) {
        if (StructType *st = dyn_cast<StructType>(dest->getElementType())) {
          unsigned numElems = st->getNumElements();
          unsigned sum = 0;
          for(unsigned i = 0; i < numElems; i++) {
            Type* type = st->getElementType(i);
            sum =+ type->getPrimitiveSizeInBits();
            cout << "YES!!!!" << sum << endl;
          }
        }
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
  if (noUnwind) {
    Value* callReturnValue = KVALUE_VALUE(callInst, instrsAfter, NOSIGN); 
    Instruction* call = CALL_KVALUE("llvm_call_nounwind", callReturnValue);
    instrsAfter.push_back(call);

    InsertAllAfter(instrsAfter, callInst);
  }

  return true;
}
