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
      Value* value = KVALUE_VALUE(phiNode->getIncomingValue(i), instrs, SIGNED);
      Constant* blockInxC = computeIndex(phiNode->getIncomingBlock(i)); 

      Instruction* call = CALL_KVALUE_INT("llvm_push_phinode_value", value, blockInxC);
      instrs.push_back(call);
    }

    Instruction* call = CALL_IID_INT("llvm_phinode", iidC, inxC);
    instrs.push_back(call);

    cout << endl << "[PHINode] => Instructions after: " << instrs.size() << endl;
    InsertAllAfter(instrs, phiNode);

    return true;
    
  } else {
    
    return false;

  }
}
