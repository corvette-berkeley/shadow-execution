/**
 * @file CmpInstrumenter.h
 * @brief CmpInstrumenter Declarations.
 */

#include "CmpInstrumenter.h"


bool CmpInstrumenter::CheckAndInstrument(Instruction* inst) {
  CmpInst* cmpInst = dyn_cast<CmpInst>(inst);

  if (cmpInst != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Value* lop = KVALUE_VALUE(cmpInst->getOperand(0U), instrs, SIGNED);
    if(lop == NULL) return false;

    Value* rop = KVALUE_VALUE(cmpInst->getOperand(1U), instrs, SIGNED);
    if(rop == NULL) return false;

    Constant* iidC = IID_CONSTANT(cmpInst);

    Constant* inxC = computeIndex(cmpInst);

    PRED pred = cmpInst->getPredicate();
    Constant* predC = PRED_CONSTANT(pred);

    std::stringstream callback;
    callback << "llvm_";
    callback << Instruction::getOpcodeName(cmpInst->getOpcode());

    Instruction* call = CALL_IID_KVALUE_KVALUE_PRED_INT(callback.str().c_str(), iidC, lop, rop, predC, inxC);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, cmpInst);

    return true;

  } else {

    return false;

  }
}
