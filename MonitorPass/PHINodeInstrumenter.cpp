/**
 * @file PHINodeInstrumenter.cpp
 * @brief
 */

#include "PHINodeInstrumenter.h"

bool PHINodeInstrumenter::CheckAndInstrument(Instruction* inst) {
  PHINode* phiNode = dyn_cast<PHINode>(inst);

  if (phiNode != NULL) {

    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Constant* iidC = IID_CONSTANT(phiNode);

    Constant* inxC = computeIndex(phiNode);

    unsigned valuePairs = phiNode->getNumIncomingValues();

    for (unsigned i = 0; i < valuePairs; i++) {
      Value* inValue = phiNode->getIncomingValue(i);
      Constant* blockInxC = computeIndex(phiNode->getIncomingBlock(i)); 
      if (dyn_cast<Constant>(inValue) != NULL) {
        Value* value = KVALUE_VALUE(inValue, instrs, SIGNED);

        Instruction* call = CALL_KVALUE_INT("llvm_push_phinode_constant_value", value, blockInxC);
        instrs.push_back(call);
      } else {
        Constant* inValueInxC = computeIndex(inValue);

        Instruction* call = CALL_INT_INT("llvm_push_phinode_value", inValueInxC, blockInxC);
        instrs.push_back(call);
      }
    }

    Instruction* call = CALL_IID_INT("llvm_phinode", iidC, inxC);
    instrs.push_back(call);

    InsertAllAfter(instrs, phiNode);

    return true;
    
  } else {
    
    return false;

  }
}
