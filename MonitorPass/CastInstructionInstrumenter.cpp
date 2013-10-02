/**
 * @file CastInstructionInstrumenter.cpp
 * @brief
 */

#include "CastInstructionInstrumenter.h"

bool CastInstructionInstrumenter::CheckAndInstrument(Instruction* inst) {

  CastInst* castInst = dyn_cast<CastInst>(inst);

  if (castInst != NULL) {
    safe_assert(parent_ != NULL);

    count_++;

    InstrPtrVector instrs;

    Value* op = KVALUE_VALUE(castInst->getOperand(0U), instrs, NOSIGN);
    if(op == NULL) return false;

    Type *type = castInst->getType();
    if (!type) return false;

    KIND kind = TypeToKind(type);
    if(kind == INV_KIND) return false;

    Constant* kindC = KIND_CONSTANT(kind);

    Constant* iidC = IID_CONSTANT(castInst);

    Constant* inxC = computeIndex(castInst);

    std::stringstream callback;
    callback << "llvm_";
    callback << Instruction::getOpcodeName(castInst->getOpcode());

    Instruction* call = CALL_IID_KIND_KVALUE_INT(callback.str().c_str(), iidC, kindC, op, inxC);

    instrs.push_back(call);

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
    InsertAllBefore(instrs, castInst);

    ////////
    /*
      verifyModule(*parent_->M_, AbortProcessAction);
      printf("Module verified after instrumenting bitcast\n");
    */
    ////////
    
    return true;
  } else {
    return false;
  }
}

CASTOP CastInstructionInstrumenter::getCastOp(CastInst* castInst) {
  if (castInst != NULL) {
    switch (castInst->getOpcode()) {
      case Instruction::CastOps(Instruction::Trunc):
        return TRUNC;
      case Instruction::CastOps(Instruction::ZExt):
        return ZEXT;
      case Instruction::CastOps(Instruction::SExt):
        return SEXT;
      case Instruction::CastOps(Instruction::FPTrunc):
        return FPTRUNC;
      case Instruction::CastOps(Instruction::FPExt):
        return FPEXT;
      case Instruction::CastOps(Instruction::FPToUI):
        return FPTOUI;
      case Instruction::CastOps(Instruction::FPToSI):
        return FPTOSI;
      case Instruction::CastOps(Instruction::UIToFP):
        return UITOFP;
      case Instruction::CastOps(Instruction::SIToFP):
        return SITOFP;
      case Instruction::CastOps(Instruction::IntToPtr):
        return INTTOPTR;
      case Instruction::CastOps(Instruction::PtrToInt):
        return PTRTOINT;
      case Instruction::CastOps(Instruction::BitCast):
        return BITCAST;
      default:
        return CASTOP_INVALID;
    }
  } else {
    return CASTOP_INVALID;
  }
}
