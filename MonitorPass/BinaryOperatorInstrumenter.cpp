/**
 * @file BinaryOperatorInstrumenter.cpp
 * @brief
 */

#include "BinaryOperatorInstrumenter.h"

bool BinaryOperatorInstrumenter::CheckAndInstrument(Instruction* inst) {
  BinaryOperator* binInst = dyn_cast<BinaryOperator>(inst);
  BINOP binop = getBinOp(binInst);
  BITWISE bitwise = getBitWise(binInst);
  if (binInst != NULL && (binop != BINOP_INVALID || bitwise != BITWISE_INVALID)) {
    safe_assert(parent_ != NULL);

    count_++;

    bool isSigned = true;    
    if (binInst->getOperand(0)->getType()->isIntegerTy(1)) {
      isSigned = false;
    }

    InstrPtrVector instrs;
    Value* lop = KVALUE_VALUE(binInst->getOperand(0), instrs, isSigned);
    if (lop == NULL) return false;

    Value *rop = KVALUE_VALUE(binInst->getOperand(1), instrs, isSigned);
    if (rop == NULL) return false;

    Constant* iid = IID_CONSTANT(binInst);
    Constant* inx = computeIndex(binInst);
    Constant* CLine = INT32_CONSTANT(getLineNumber(binInst), SIGNED);

    Constant* nuw = BOOL_CONSTANT(binInst->hasNoUnsignedWrap());
    Constant* nsw = BOOL_CONSTANT(binInst->hasNoSignedWrap());

    std::stringstream callback;

    switch(binop) {
      case ADD:
        callback << "llvm_add";
        break;
      case SUB:
        callback << "llvm_sub";
        break;
      case MUL:
        callback << "llvm_mul";
        break;
      case UDIV:
        callback << "llvm_udiv";
        break;
      case SDIV:
        callback << "llvm_sdiv";
        break;
      case UREM:
        callback << "llvm_urem";
        break;
      case SREM:
        callback << "llvm_srem";
        break;
      case FADD:
        callback << "llvm_fadd";
        break;
      case FSUB:
        callback << "llvm_fsub";
        break;
      case FMUL:
        callback << "llvm_fmul";
        break;
      case FDIV:
        callback << "llvm_fdiv";
        break;
      case FREM:
        callback << "llvm_frem";
        break;
      case BINOP_INVALID:
        switch (bitwise) {
          case SHL:
            callback << "llvm_shl";
            break;
          case LSHR:
            callback << "llvm_lshr";
            break;
          case ASHR:
            callback << "llvm_ashr";
            break;
          case AND:
            callback << "llvm_and_";
            break;
          case OR:
            callback << "llvm_or_";
            break;
          case XOR:
            callback << "llvm_xor_";
            break;
          default:
            safe_assert(false);
            return false; // this cannot happen
        }
        break;
      default:
        safe_assert(false);
        return false; // this cannot happen
    }

    Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE_INT_INT(callback.str().c_str(), iid, nuw, nsw, lop, rop, CLine, inx);
    instrs.push_back(call);

    // instrument
    InsertAllBefore(instrs, binInst);

    return true;
  } else {
    return false;
  } 
}

BINOP BinaryOperatorInstrumenter::getBinOp(BinaryOperator* binInst) {
  if (binInst != NULL) {
    switch(binInst->getOpcode()) {
      case Instruction::BinaryOps(Instruction::Add): 
        return ADD;
      case Instruction::BinaryOps(Instruction::Sub): 
        return SUB;
      case Instruction::BinaryOps(Instruction::Mul): 
        return MUL;
      case Instruction::BinaryOps(Instruction::UDiv): 
        return UDIV;
      case Instruction::BinaryOps(Instruction::SDiv): 
        return SDIV;
      case Instruction::BinaryOps(Instruction::URem): 
        return UREM;
      case Instruction::BinaryOps(Instruction::SRem): 
        return SREM;
      case Instruction::BinaryOps(Instruction::FAdd): 
        return FADD;
      case Instruction::BinaryOps(Instruction::FSub): 
        return FSUB;
      case Instruction::BinaryOps(Instruction::FMul): 
        return FMUL;
      case Instruction::BinaryOps(Instruction::FDiv): 
        return FDIV;
      case Instruction::BinaryOps(Instruction::FRem): 
        return FREM;
      default:
        return BINOP_INVALID;
    }
  } else {
    return BINOP_INVALID;
  }
}

BITWISE BinaryOperatorInstrumenter::getBitWise(BinaryOperator* binInst) {
  if (binInst != NULL) {
    switch(binInst->getOpcode()) {
      case Instruction::BinaryOps(Instruction::Shl): 
        return SHL;
      case Instruction::BinaryOps(Instruction::LShr): 
        return LSHR;
      case Instruction::BinaryOps(Instruction::AShr): 
        return ASHR;
      case Instruction::BinaryOps(Instruction::And): 
        return AND;
      case Instruction::BinaryOps(Instruction::Or): 
        return OR;
      case Instruction::BinaryOps(Instruction::Xor): 
        return XOR;
      default:
        return BITWISE_INVALID;
    }
  } else {
    return BITWISE_INVALID;
  }
}
