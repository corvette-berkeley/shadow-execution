/**
 * @file StoreInstrumenter.cpp
 * @brief
 */

#include "StoreInstrumenter.h"

bool StoreInstrumenter::CheckAndInstrument(Instruction* inst) {
  StoreInst* storeInst = dyn_cast<StoreInst>(inst);

  if (storeInst != NULL) {

    // skip stores from main if operands are arguments
    if (BasicBlock *basicBlock = storeInst->getParent()) {
      if (Function *function = basicBlock->getParent()) {
        if (function->getName() == "main") {
          if (dyn_cast<Argument>(storeInst->getValueOperand())) {
            return false;
          }
        }
      }
    }

    safe_assert(parent_ != NULL);

    count_++;

    parent_->AS_ = storeInst->getPointerAddressSpace();

    InstrPtrVector instrs;
    Value* valueOp = KVALUE_VALUE(storeInst->getValueOperand(),
        instrs, SIGNED);
    if(valueOp == NULL) return false; 

    Value* pointerOp = KVALUE_VALUE(storeInst->getPointerOperand(),
        instrs, NOSIGN);
    if(pointerOp == NULL) return false; 

    Constant* iidC = IID_CONSTANT(storeInst);

    Constant* inxC = computeIndex(storeInst);

    Constant* lineC = INT32_CONSTANT(getLineNumber(storeInst), SIGNED);

    string filename = getFileName(storeInst);
    cout << filename << endl;

    
    if (parent_->fileNames.insert(std::make_pair(filename, parent_->fileCount)).second) {
      // element was inserted
      parent_->fileCount++;
    }

    Instruction* call = CALL_IID_KVALUE_KVALUE_INT_INT("llvm_store", iidC, pointerOp, valueOp, lineC, inxC);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, storeInst);

    return true;

  } else {

    return false;

  }
}
