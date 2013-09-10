// this class includes all instrumenters


#ifndef INSTRUMENTERS_H_
#define INSTRUMENTERS_H_

#include "Common.h"
#include "Instrumenter.h"
#include <llvm/Analysis/Verifier.h>

/*******************************************************************************************/

#define INSTR_TO_CALLBACK(inst)		("llvm_" inst)

/*******************************************************************************************/



/*******************************************************************************************/
// Callback: void load(IID iid, PTR addr, KVALUE value)
class LoadInstrumenter : public Instrumenter {
public:
	DEFAULT_CONSTRUCTOR(LoadInstrumenter);

	bool CheckAndInstrument(Instruction* I) {
		CAST_OR_RETURN(LoadInst, LI, I);

		safe_assert(parent_ != NULL);

		count_++;

		parent_->AS_ = LI->getPointerAddressSpace();

		InstrPtrVector Instrs;
		Value* kvalue = KVALUE_VALUE(LI, Instrs, NOSIGN);
		if(kvalue == NULL) return false;

		Constant* C_iid = IID_CONSTANT(LI);
		Instruction* I_cast_ptr = PTR_CAST_INSTR(LI->getPointerOperand());
		Instrs.push_back(I_cast_ptr);

		Instruction* call = CALL_IID_PTR_KVALUE(INSTR_TO_CALLBACK("load"), C_iid, I_cast_ptr, kvalue);
		Instrs.push_back(call);
		//errs() << "===Number of instructions to add: " << Instrs.size() << "\n";

		// instrument
		InsertAllAfter(Instrs, LI);

		return true;
	}
};

/*******************************************************************************************/

// Callback: void add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
class AddInstrumenter : public Instrumenter {
public:
	DEFAULT_CONSTRUCTOR(AddInstrumenter);

	bool CheckAndInstrument(Instruction* I) {
		CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, Add);

		safe_assert(parent_ != NULL);

		count_++;

		InstrPtrVector Instrs;
		Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
		if(op1 == NULL) return false;

		Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
		if(op2 == NULL) return false;

		Constant* C_iid = IID_CONSTANT(BI);

		Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
		Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

		Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("add"), C_iid, nuw, nsw, op1, op2);
		Instrs.push_back(call);

		// instrument
		InsertAllBefore(Instrs, BI);

    return true;
  }
};


// Callback: void fadd()
class FAddInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FAddInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, FAdd);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("fadd"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      //		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_fadd"), FunctionType::get(VOID_TYPE(), false)));
      //		call->insertBefore(I);

      return true;
    }
};


/*******************************************************************************************/

// Callback: void sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
class SubInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(SubInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, Sub);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("sub"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void fsub()
class FSubInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FSubInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, FSub);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("fsub"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void mul()
class MulInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(MulInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, Mul);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("mul"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);


      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void fmul()
class FMulInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FMulInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, FMul);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("fmul"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void udiv()
class UDivInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(UDivInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, UDiv);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("udiv"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void sdiv()
class SDivInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(SDivInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, SDiv);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("sdiv"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void fdiv()
class FDivInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FDivInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, FDiv);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("fdiv"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};

// Callback: void urem()
class URemInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(URemInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, URem);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("urem"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void srem()
class SRemInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(SRemInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, SRem);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("srem"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void frem()
class FRemInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FRemInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, FRem);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("frem"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// ***** Bitwise Binary Operations ***** //

// Callback: void shl()
class ShlInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(ShlInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, Shl);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("shl"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void lshr()
class LShrInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(LShrInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, LShr);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("lshr"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void ashr()
class AShrInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(AShrInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, AShr);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("ashr"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void and()
class AndInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(AndInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, And);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("and_"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};

// Callback: void or()
class OrInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(OrInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, Or);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("or_"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};


// Callback: void xor()
class XorInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(XorInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_BINARY_OR_RETURN(BinaryOperator, BI, I, Xor);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;
      Value* op1 = KVALUE_VALUE(BI->getOperand(0U), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Value* op2 = KVALUE_VALUE(BI->getOperand(1U), Instrs, NOSIGN);
      if(op2 == NULL) return false;

      Constant* C_iid = IID_CONSTANT(BI);

      Constant* nuw = BOOL_CONSTANT(BI->hasNoUnsignedWrap());
      Constant* nsw = BOOL_CONSTANT(BI->hasNoSignedWrap());

      Instruction* call = CALL_IID_BOOL_BOOL_KVALUE_KVALUE(INSTR_TO_CALLBACK("xor_"), C_iid, nuw, nsw, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, BI);

      return true;
    }
};



/*******************************************************************************************/

// Callback: void extractelement()
class ExtractElementInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(ExtractElementInstrumenter);

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

      Instruction* call = CALL_IID_KVALUE_KVALUE(INSTR_TO_CALLBACK("extractelement"), C_iid, op1, op2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};



// Callback: void insertelement()
class InsertElementInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(InsertElementInstrumenter);

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
    DEFAULT_CONSTRUCTOR(ShuffleVectorInstrumenter);

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
    DEFAULT_CONSTRUCTOR(ExtractValueInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(InsertValueInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Value* op1 = KVALUE_VALUE(SI->getAggregateOperand(), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("extractvalue"), C_iid, op1);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void insertvalue()
class InsertValueInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(InsertValueInstrumenter);

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

      Instruction* call = CALL_IID_KVALUE_KVALUE(INSTR_TO_CALLBACK("insertvalue"), C_iid, op1, op2);
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
    DEFAULT_CONSTRUCTOR(AllocaInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(AllocaInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      //Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      //if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Type *T = SI->getAllocatedType();
      if (!T) return false;
      KIND kind = TypeToKind(T);
      // if unsupported kind, return false
      if(kind == INV_KIND) return false;
      Constant* C_kind = KIND_CONSTANT(kind);

      Instruction* call = CALL_IID_KIND(INSTR_TO_CALLBACK("allocax"), C_iid, C_kind);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

/*
// Callback: void load()
class LoadInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(LoadInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(LoadInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_load"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};
*/

// Callback: void store(IID iid, PTR addr, KVALUE value)
class StoreInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(StoreInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(StoreInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      parent_->AS_ = SI->getPointerAddressSpace();

      InstrPtrVector Instrs;
      Value* kvalue = KVALUE_VALUE(SI->getValueOperand(), Instrs, NOSIGN);
      if(kvalue == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);
      Instruction* I_cast_ptr = PTR_CAST_INSTR(SI->getPointerOperand());
      Instrs.push_back(I_cast_ptr);

      Instruction* call = CALL_IID_PTR_KVALUE(INSTR_TO_CALLBACK("store"), C_iid, I_cast_ptr, kvalue);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, I);

      return true;
    }
};


// Callback: void fence()
class FenceInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FenceInstrumenter);

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
    DEFAULT_CONSTRUCTOR(AtomicCmpXchgInstrumenter);

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

      Instruction* call = CALL_IID_PTR_KVALUE_KVALUE(INSTR_TO_CALLBACK("cmpxchg"), C_iid, I_cast_ptr, kvalue1, kvalue2);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, I);

      return true;
    }
};


// Callback: void atomicrmw()
class AtomicRMWInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(AtomicRMWInstrumenter);

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
    DEFAULT_CONSTRUCTOR(GetElementPtrInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(GetElementPtrInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Constant* inbound = BOOL_CONSTANT(SI->isInBounds());

      Value* op1 = KVALUE_VALUE(SI->getPointerOperand(), Instrs, NOSIGN);
      if(op1 == NULL) return false;

      Instruction* call = CALL_IID_BOOL_KVALUE(INSTR_TO_CALLBACK("getelementptr"), C_iid, inbound, op1);
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
    DEFAULT_CONSTRUCTOR(TruncInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(TruncInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("trunc"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void zext()
class ZExtInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(ZExtInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ZExtInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(1U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("zext"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void sext()
class SExtInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(SExtInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SExtInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("sext"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fptrunc()
class FPTruncInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FPTruncInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPTruncInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("fptrunc"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fpext()
class FPExtInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FPExtInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPExtInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("fpext"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fptoui()
class FPToUIInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FPToUIInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPToUIInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("fptoui"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void fptosi()
class FPToSIInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FPToSIInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(FPToSIInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("fptosi"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void uitofp()
class UIToFPInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(UIToFPInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(UIToFPInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("uitofp"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void sitofp()
class SIToFPInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(SIToFPInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SIToFPInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("sitofp"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void ptrtoint()
class PtrToIntInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(PtrToIntInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(PtrToIntInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("ptrtoint"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void inttoptr()
class IntToPtrInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(IntToPtrInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(IntToPtrInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("inttoptr"), C_iid, op);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void bitcast()
class BitCastInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(BitCastInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(BitCastInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Value* op = KVALUE_VALUE(SI->getOperand(0U), Instrs, NOSIGN);
      if(op == NULL) return false;

      Constant* C_iid = IID_CONSTANT(SI);

      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("bitcast"), C_iid, op);
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
    DEFAULT_CONSTRUCTOR(BranchInstrumenter);

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
        Instruction* call = CALL_IID_BOOL_KVALUE(INSTR_TO_CALLBACK("branch"), C_iid, conditional, op1);
        Instrs.push_back(call);
      } else {
        Instruction* call = CALL_IID_BOOL(INSTR_TO_CALLBACK("branch2"), C_iid, conditional);
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
    DEFAULT_CONSTRUCTOR(IndirectBrInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(IndirectBrInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);
      Value* op1 = KVALUE_VALUE(SI->getAddress(), Instrs, NOSIGN);
      if (op1 == NULL) return false;
      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("indirectbr"), C_iid, op1);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void invoke()
class InvokeInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(InvokeInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(InvokeInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_invoke"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};


// Callback: void resume()
class ResumeInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(ResumeInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ResumeInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);
      Value* op1 = KVALUE_VALUE(SI->getValue(), Instrs, NOSIGN);
      if (op1 == NULL) return false;
      Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("resume"), C_iid, op1);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);


      return true;
    }
};


// Callback: void return()
class ReturnInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(ReturnInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(ReturnInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);
      Value* retVal = SI->getReturnValue();

      if (retVal == NULL) {
        Instruction* call = CALL_IID(INSTR_TO_CALLBACK("return2_"), C_iid);
        Instrs.push_back(call);
      } else {
        Value* op1 = KVALUE_VALUE(retVal, Instrs, NOSIGN);
        if (op1 == NULL) return false;
        Instruction* call = CALL_IID_KVALUE(INSTR_TO_CALLBACK("return_"), C_iid, op1);
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
    DEFAULT_CONSTRUCTOR(SwitchInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SwitchInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_switch_"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};


// Callback: void unreachable()
class UnreachableInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(UnreachableInstrumenter);

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
    DEFAULT_CONSTRUCTOR(ICmpInstrumenter);

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

      Instruction* call = CALL_IID_KVALUE_KVALUE_PRED(INSTR_TO_CALLBACK("icmp"), C_iid, op1, op2, C_pred);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);
      
      return true;
    }
};


// Callback: void fcmp()
class FCmpInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(FCmpInstrumenter);

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

      Instruction* call = CALL_IID_KVALUE_KVALUE_PRED(INSTR_TO_CALLBACK("icmp"), C_iid, op1, op2, C_pred);
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};

// Callback: void phinode()
class PHINodeInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(PHINodeInstrumenter);

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
    DEFAULT_CONSTRUCTOR(SelectInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SelectInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_select"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};

// Callback: void call()
class CallInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(CallInstrumenter);

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(CallInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_call"), FunctionType::get(VOID_TYPE(), false)));
      call->insertBefore(I);

      return true;
    }
};


// Callback: void vaarg()
class VAArgInstrumenter : public Instrumenter {
  public:
    DEFAULT_CONSTRUCTOR(VAArgInstrumenter);

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
    DEFAULT_CONSTRUCTOR(LandingPadInstrumenter);

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

