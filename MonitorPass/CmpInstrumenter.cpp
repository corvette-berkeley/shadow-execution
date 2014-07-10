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
    Value *lValue, *rValue;
    Constant *cInx, *cType, *cLScope, *cRScope, *cPred;
    Value *cLValue, *cRValue;
    std::stringstream callback;
    PRED pred;
    Instruction *call;

    lValue = cmpInst->getOperand(0);
    rValue = cmpInst->getOperand(1);

    cInx = computeIndex(cmpInst);
    cType = KIND_CONSTANT(TypeToKind(cmpInst->getType()));

    cLScope = INT32_CONSTANT(getScope(lValue), SIGNED);
    cRScope = INT32_CONSTANT(getScope(rValue), SIGNED);

    if (isa<Constant>(lValue) && TypeToKind(lValue->getType()) == PTR_KIND) {
      cLValue = PTRTOINT_CAST_INSTR(lValue);
      instrs.push_back((Instruction *) cLValue);
    } else if (isa<ConstantFP>(lValue) && TypeToKind(lValue->getType()) == FLP80X86_KIND) {
      Instruction *fcast = FLPMAX_CAST_INSTR(lValue);
      instrs.push_back(fcast);
      cLValue = VALUE_CAST_INSTR(fcast);
      instrs.push_back((Instruction *) cLValue);
    } else {
      cLValue = getValueOrIndex(lValue);
    }

    if (isa<Constant>(rValue) && TypeToKind(rValue->getType()) == PTR_KIND) {
      cRValue = PTRTOINT_CAST_INSTR(rValue);
      instrs.push_back((Instruction *) cRValue);
    } else if (isa<ConstantFP>(rValue) && TypeToKind(rValue->getType()) == FLP80X86_KIND) {
      Instruction *fcast = FLPMAX_CAST_INSTR(rValue);
      instrs.push_back(fcast);
      cRValue = VALUE_CAST_INSTR(fcast);
      instrs.push_back((Instruction *) cRValue);
    } else {
      cRValue = getValueOrIndex(rValue);
    }

      pred = cmpInst->getPredicate();
    cPred = PRED_CONSTANT(pred);

    callback << "llvm_";
    callback << Instruction::getOpcodeName(cmpInst->getOpcode());

    call = CALL_INT_INT_INT64_INT64_KIND_PRED_INT(callback.str().c_str(),
        cLScope, cRScope, cLValue, cRValue, cType, cPred, cInx);
      
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, cmpInst);

    return true;

  } else {

    return false;

  }
}
