/**
 * @file StoreInstrumenter.cpp
 * @brief
 */

#include "StoreInstrumenter.h"

bool StoreInstrumenter::CheckAndInstrument(Instruction* inst) {
  StoreInst* storeInst; 
  
  storeInst = dyn_cast<StoreInst>(inst);
  if (storeInst != NULL) {
    safe_assert(parent_ != NULL);

    InstrPtrVector instrs;
    Value *valueOp;
    Constant *cPointerInx, *cScope, *cInx, *cLine, *cFile;
    string filename;
    Instruction *call;

    count_++;

    //
    // skip stores in main if operands are arguments
    //
    if (BasicBlock *basicBlock = storeInst->getParent()) {
      if (Function *function = basicBlock->getParent()) {
        if (function->getName() == "main") {
          if (dyn_cast<Argument>(storeInst->getValueOperand())) {
            return false;
          }
        }
      }
    }

    parent_->AS_ = storeInst->getPointerAddressSpace();

    valueOp = KVALUE_VALUE(storeInst->getValueOperand(),
        instrs, SIGNED);
    if(valueOp == NULL) return false; 

    Value* pointerOp = storeInst->getPointerOperand();

    cPointerInx = computeIndex(pointerOp);
    cScope = INT32_CONSTANT(getScope(pointerOp), SIGNED);
    cInx = computeIndex(storeInst);
    cLine = INT32_CONSTANT(getLineNumber(storeInst), SIGNED);

    filename = getFileName(storeInst);
    if (parent_->fileNames.insert(std::make_pair(filename, parent_->fileCount)).second) {
      // element was inserted
      cFile = INT32_CONSTANT(parent_->fileCount, SIGNED);
      parent_->fileCount++;
    }
    else {
      cFile = INT32_CONSTANT(parent_->fileNames[filename], SIGNED);
    }

    call = CALL_INT_INT_KVALUE_INT_INT_INT("llvm_store", cPointerInx, cScope, valueOp, cFile, cLine, cInx);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, storeInst);

    return true;

  } else {

    return false;

  }
}

SCOPE StoreInstrumenter::getScope(Value *value) {
  if (isa<GlobalVariable>(value)) {
    return GLOBAL;
  } else if (isa<Constant>(value)) {
    return CONSTANT;
  } else {
    safe_assert(isa<Instruction>(value) || isa<Argument>(value));
    return LOCAL;
  }
}
