// this class includes all instrumenters


#ifndef INSTRUMENTERS_H_
#define INSTRUMENTERS_H_

#include "Common.h"
#include "Instrumenter.h"
#include "BinaryOperatorInstrumenter.h"
#include <llvm/Analysis/Verifier.h>

/*******************************************************************************************/

#define INSTR_TO_CALLBACK(inst)		("llvm_" inst)

/*******************************************************************************************/

/*******************************************************************************************/

// Callback: void extractelement()
class ExtractElementInstrumenter : public Instrumenter {
  public:
    ExtractElementInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ExtractElementInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Value* op1 = KVALUE_VALUE(SI->getVectorOperand(), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(SI->getIndexOperand(), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Instruction* call = CALL_IID_KVALUE_KVALUE_INT(INSTR_TO_CALLBACK("extractelement"), C_iid, op1, op2, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void insertelement()
class InsertElementInstrumenter : public Instrumenter {
  public:
    InsertElementInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(InsertElementInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_insertelement"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};

// Callback: void shufflevector()
class ShuffleVectorInstrumenter : public Instrumenter {
  public:
    ShuffleVectorInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ShuffleVectorInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_shufflevector"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};


// ***** AGGREGATE OPERATIONS ***** //

// Callback: void extractvalue()
class ExtractValueInstrumenter : public Instrumenter {
  public:
    ExtractValueInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(InsertValueInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Value* op1 = KVALUE_VALUE(SI->getAggregateOperand(), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("extractvalue"), C_iid, op1, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void insertvalue()
class InsertValueInstrumenter : public Instrumenter {
  public:
    InsertValueInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(InsertValueInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

     InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Value* op1 = KVALUE_VALUE(SI->getAggregateOperand(), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(SI->getInsertedValueOperand(), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Instruction* call = CALL_IID_KVALUE_KVALUE_INT(INSTR_TO_CALLBACK("insertvalue"), C_iid, op1, op2, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// ***** Memory Access and Addressing Operations ***** //

// Callback: void alloca()
class AllocaInstrumenter : public Instrumenter {
  public:
    AllocaInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(AllocaInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Type *T = SI->getAllocatedType();
      if (!T) return false;
      KIND kind = TypeToKind(T);

      // if unsupported kind, return false
      if (kind == INV_KIND) return false;

      Constant* size;
      if (T->isArrayTy()) {
        ArrayType* aType = (ArrayType*) T;
        size = INT64_CONSTANT(aType->getNumElements(), UNSIGNED);
      } else {
        size = INT64_CONSTANT(0, UNSIGNED);
      }

      Constant* C_kind = KIND_CONSTANT(kind);

      Instruction* call = CALL_IID_KIND_INT64_INT(INSTR_TO_CALLBACK("allocax"), C_iid, C_kind, size, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void store(IID iid, PTR addr, KVALUE value)
class StoreInstrumenter : public Instrumenter {
  public:
    StoreInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(StoreInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      parent_->AS_ = SI->getPointerAddressSpace();

      InstrPtrVector Instrs;
      Value* kvalue = KVALUE_VALUE(SI->getValueOperand(), Instrs, NOSIGN);
      if(kvalue == NULL) {
        return false; 
      }

      Constant* C_iid = IID_CONSTANT(SI);
      Instruction* I_cast_ptr = PTR_CAST_INSTR(SI->getPointerOperand());
      Instrs.push_back(I_cast_ptr);

      // cuong: pass a kvalue instead of pointer value
      Value* op = KVALUE_VALUE(SI->getPointerOperand(), Instrs, NOSIGN);
      
      // new: iid for ptr
      // Instruction *ptr_iid = (Instruction*)(SI->getPointerOperand());
      // Constant* C_ptr_iid = IID_CONSTANT(ptr_iid);

      Instruction* call = CALL_IID_KVALUE_KVALUE_INT(INSTR_TO_CALLBACK("store"), C_iid, op, kvalue, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, I);

      return true;
    }
};

// Callback: void fence()
class FenceInstrumenter : public Instrumenter {
  public:
    FenceInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FenceInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_fence"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};


// Callback: void cmpxchg()
class AtomicCmpXchgInstrumenter : public Instrumenter {
  public:
    AtomicCmpXchgInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(AtomicCmpXchgInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* kvalue1 = KVALUE_VALUE(SI->getCompareOperand(), Instrs, NOSIGN);
      if(kvalue1 == NULL) return false;

      Value* kvalue2 = KVALUE_VALUE(SI->getNewValOperand(), Instrs, NOSIGN);
      if(kvalue2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);
      Instruction* I_cast_ptr = PTR_CAST_INSTR(SI->getPointerOperand());
      Instrs.push_back(I_cast_ptr);

      Instruction* call = CALL_IID_PTR_KVALUE_KVALUE_INT(INSTR_TO_CALLBACK("cmpxchg"), C_iid, I_cast_ptr, kvalue1, kvalue2, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, I);

      return true;
    }
};


// Callback: void atomicrmw()
class AtomicRMWInstrumenter : public Instrumenter {
  public:
    AtomicRMWInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(AtomicRMWInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_atomicrmw"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};


// Callback: void getelementptr()
class GetElementPtrInstrumenter : public Instrumenter {
  public:
    GetElementPtrInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(GetElementPtrInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Constant* inbound = BOOL_CONSTANT(SI->isInBounds());

      Value* op1 = KVALUE_VALUE(SI->getPointerOperand(), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      PointerType* T = (PointerType*) SI->getPointerOperandType();
      Type* elemT = T->getElementType();
      KIND kind = TypeToKind(elemT);

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* size;
      if (elemT->isArrayTy()) {
        ArrayType* aType = (ArrayType*) T;
        size = INT64_CONSTANT(aType->getNumElements(), UNSIGNED);
      } else {
        size = INT64_CONSTANT(0, UNSIGNED);
      }

      Instruction* call = CALL_IID_BOOL_KVALUE_KIND_INT64_INT(INSTR_TO_CALLBACK("getelementptr"), C_iid, inbound, op1, C_kind, size, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// ***** Conversion Operations ***** //

// Callback: void trunc()
class TruncInstrumenter : public Instrumenter {
  public:
    TruncInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(TruncInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("trunc"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void zext()
class ZExtInstrumenter : public Instrumenter {
  public:
    ZExtInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ZExtInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(1U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("zext"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void sext()
class SExtInstrumenter : public Instrumenter {
  public:
    SExtInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SExtInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("sext"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fptrunc()
class FPTruncInstrumenter : public Instrumenter {
  public:
    FPTruncInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPTruncInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("fptrunc"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fpext()
class FPExtInstrumenter : public Instrumenter {
  public:
    FPExtInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPExtInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("fpext"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fptoui()
class FPToUIInstrumenter : public Instrumenter {
  public:
    FPToUIInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPToUIInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("fptoui"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fptosi()
class FPToSIInstrumenter : public Instrumenter {
  public:
    FPToSIInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPToSIInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("fptosi"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void uitofp()
class UIToFPInstrumenter : public Instrumenter {
  public:
    UIToFPInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(UIToFPInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("uitofp"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void sitofp()
class SIToFPInstrumenter : public Instrumenter {
  public:
    SIToFPInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SIToFPInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("sitofp"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void ptrtoint()
class PtrToIntInstrumenter : public Instrumenter {
  public:
    PtrToIntInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(PtrToIntInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("ptrtoint"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void inttoptr()
class IntToPtrInstrumenter : public Instrumenter {
  public:
    IntToPtrInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(IntToPtrInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("inttoptr"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void bitcast()
class BitCastInstrumenter : public Instrumenter {
  public:
    BitCastInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(BitCastInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Type *T = SI->getType();
      if (!T) return false;

      KIND kind = TypeToKind(T);
      if(kind == INV_KIND) return false;

      Constant* C_kind = KIND_CONSTANT(kind);

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KIND_KVALUE_INT(INSTR_TO_CALLBACK("bitcast"), C_iid, C_kind, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// ***** TerminatorInst ***** //

// Callback: void branch()
class BranchInstrumenter : public Instrumenter {
  public:
    BranchInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(BranchInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* conditional = BOOL_CONSTANT(SI->isConditional());
      Constant* C_iid = IID_CONSTANT(SI);

      if (SI->isConditional()) {
        Value* op1 = KVALUE_VALUE(SI->getCondition(), Instrs, NOSIGN);
        if(op1 == NULL) return false;
        Instruction* call = CALL_IID_BOOL_KVALUE_INT(INSTR_TO_CALLBACK("branch"), C_iid, conditional, op1, computeIndex(SI));
        Instrs.push_back(call);
      } else {
        Instruction* call = CALL_IID_BOOL_INT(INSTR_TO_CALLBACK("branch2"), C_iid, conditional, computeIndex(SI));
        Instrs.push_back(call);
      }

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void indirectbr()
class IndirectBrInstrumenter : public Instrumenter {
  public:
    IndirectBrInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(IndirectBrInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);
      Value* op1 = KVALUE_VALUE(SI->getAddress(), Instrs, NOSIGN);
      if (op1 == NULL) return false;
      Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("indirectbr"), C_iid, op1, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void invoke()
class InvokeInstrumenter : public Instrumenter {
  public:
    InvokeInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(InvokeInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      // get call arguments
      unsigned numArgs = SI->getNumArgOperands();
      unsigned i;

      for (i = 0; i < numArgs; i++)
      {
        Value* arg = KVALUE_VALUE(SI->getArgOperand(i), Instrs, NOSIGN);
        Instruction* call = CALL_KVALUE(INSTR_TO_CALLBACK("push_stack"), arg);
        Instrs.push_back(call);
      }

      Value* call_value = KVALUE_VALUE(SI->getCalledValue(), Instrs, NOSIGN);

      Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("invoke"), C_iid, call_value, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;

    }
};


// Callback: void resume()
class ResumeInstrumenter : public Instrumenter {
  public:
    ResumeInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ResumeInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);
      Value* op1 = KVALUE_VALUE(SI->getValue(), Instrs, NOSIGN);
      if (op1 == NULL) return false;
      Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("resume"), C_iid, op1, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);


      return true;
    }
};


// Callback: void return()
class ReturnInstrumenter : public Instrumenter {
  public:
    ReturnInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ReturnInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);
      Value* retVal = SI->getReturnValue();

      if (retVal == NULL) {
        Instruction* call = CALL_IID_INT(INSTR_TO_CALLBACK("return2_"), C_iid, computeIndex(SI));
        Instrs.push_back(call);
      } else {
        Value* op1 = KVALUE_VALUE(retVal, Instrs, NOSIGN);
        if (op1 == NULL) return false;
        Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("return_"), C_iid, op1, computeIndex(SI));
        Instrs.push_back(call);
      }

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void switch_()
class SwitchInstrumenter : public Instrumenter {
  public:
    SwitchInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SwitchInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Value* op = SI->getCondition();

      Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("switch_"), C_iid, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void unreachable()
class UnreachableInstrumenter : public Instrumenter {
  public:
    UnreachableInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(UnreachableInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_unreachable"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};


// ***** Other Operations ***** //

// Callback: void icmp()
class ICmpInstrumenter : public Instrumenter {
  public:
    ICmpInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ICmpInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op1 = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(SI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      PRED pred = SI->getUnsignedPredicate();
      Constant* C_pred = PRED_CONSTANT(pred);

      Instruction* call = CALL_IID_KVALUE_KVALUE_PRED_INT(INSTR_TO_CALLBACK("icmp"), C_iid, op1, op2, C_pred, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fcmp()
class FCmpInstrumenter : public Instrumenter {
  public:
    FCmpInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FCmpInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op1 = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(SI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      PRED pred = SI->getPredicate();
      Constant* C_pred = PRED_CONSTANT(pred);

      Instruction* call = CALL_IID_KVALUE_KVALUE_PRED_INT(INSTR_TO_CALLBACK("icmp"), C_iid, op1, op2, C_pred, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void phinode()
class PHINodeInstrumenter : public Instrumenter {
  public:
    PHINodeInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(PHINode, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_phinode"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};

// Callback: void select()
class SelectInstrumenter : public Instrumenter {
  public:
    SelectInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SelectInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* cond = KVALUE_VALUE(SI->getCondition(), Instrs, NOSIGN);
      if(cond == NULL) return false;

      Value* tvalue = KVALUE_VALUE(SI->getTrueValue(), Instrs, NOSIGN);
      if(tvalue == NULL) return false;

      Value* fvalue = KVALUE_VALUE(SI->getFalseValue(), Instrs, NOSIGN);
      if(fvalue == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE_KVALUE_KVALUE_INT(INSTR_TO_CALLBACK("select"), C_iid, cond, tvalue, fvalue, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void vaarg()
class VAArgInstrumenter : public Instrumenter {
  public:
    VAArgInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(VAArgInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_vaarg"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};

// Callback: void landingpad()
class LandingPadInstrumenter : public Instrumenter {
  public:
    LandingPadInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(LandingPadInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_landingpad"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};

#endif // INSTRUMENTERS_H_

