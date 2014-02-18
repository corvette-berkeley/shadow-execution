/**
 * @file CastInstructionInstrumenter.cpp
 * @brief
 */

#include "CastInstructionInstrumenter.h"

bool CastInstructionInstrumenter::CheckAndInstrument(Instruction* inst) {

  CastInst* castInst; 
  
  castInst = dyn_cast<CastInst>(inst);

  if (castInst != NULL) {
    safe_assert(parent_ != NULL);

    InstrPtrVector instrs;
    Value *op; 
    Constant *cOp, *cOpScope, *cOpKind, *cKind, *cSize, *cInx;
    Instruction* call;
    std::stringstream callback;

    if (PointerType* dest = dyn_cast<PointerType>(castInst->getDestTy())) {
      cSize = INT32_CONSTANT(dest->getElementType()->getPrimitiveSizeInBits()/8, false);
    } else {
      cSize = INT32_CONSTANT(0, false);
    }

    count_++;

    op = castInst->getOperand(0U);

    cOp = getValueOrIndex(op);
    cOpScope = INT32_CONSTANT(getScope(op), SIGNED);
    cOpKind = KIND_CONSTANT(TypeToKind(op->getType()));
    cKind = KIND_CONSTANT(TypeToKind(castInst->getType()));
    cInx = computeIndex(castInst);

    callback << "llvm_";
    callback << Instruction::getOpcodeName(castInst->getOpcode());

    call = CALL_INT64_INT_KIND_KIND_INT_INT(callback.str().c_str(), cOp, cOpScope, cOpKind, cKind, cSize, cInx);

    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, castInst);
    
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
