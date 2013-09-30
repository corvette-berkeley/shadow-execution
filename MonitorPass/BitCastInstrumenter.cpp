#include "BitCastInstrumenter.h"

bool BitCastInstrumenter::CheckAndInstrument(Instruction *inst) {

  BitCastInst* bitcastInst  = dyn_cast<BitCastInst>(inst);  

  if (bitcastInst != NULL) {
    safe_assert(parent_ != NULL);
    
    count_++;
  
    InstrPtrVector Instrs;
    
    Value* op = KVALUE_VALUE(bitcastInst->getOperand(0U), Instrs, NOSIGN);
    if(op == NULL) return false;
    
    Type *T = bitcastInst->getType();
    if (!T) return false;
    
    KIND kind = TypeToKind(T);
    if(kind == INV_KIND) return false;
    
    Constant* C_kind = KIND_CONSTANT(kind);
    
    Constant* C_iid = IID_CONSTANT(bitcastInst);
    
    Instruction* call = CALL_IID_KIND_KVALUE_INT("llvm_bitcast", C_iid, C_kind, op, computeIndex(bitcastInst));
  
    Instrs.push_back(call);
  
    /////////
    /*
      printf("Printing new instructions to add\n");
      for(unsigned int i = 0; i < Instrs.size(); i++) {
      Instrs[i]->dump();
      }
      
      printf("About to add instrumentation for bitcast\n");
      SI->dump();
      parent_->BB_->dump();
    */
    /////////
    
    // instrument
    InsertAllBefore(Instrs, bitcastInst);
    
    ////////
    /*
      verifyModule(*parent_->M_, AbortProcessAction);
      printf("Module verified after instrumenting bitcast\n");
    */
    ////////
    
    return true;
  }
  else {
    return false;
  }
}
