// this class includes all instrumenters


#ifndef INSTRUMENTERS_H_
#define INSTRUMENTERS_H_

#include "Common.h"
#include "Instrumenter.h"

/*******************************************************************************************/

#define INSTR_TO_CALLBACK(inst)		("llvm_" inst)

/*******************************************************************************************/


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

/*******************************************************************************************/
/*
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

		// instrument
		InsertAllAfter(Instrs, LI);

		return true;
	}
};
*/
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

/*******************************************************************************************/

// Callback: void alloca()
class AllocaInstrumenter : public Instrumenter {
public:
	DEFAULT_CONSTRUCTOR(AllocaInstrumenter);

	bool CheckAndInstrument(Instruction* I) {
		CAST_OR_RETURN(AllocaInst, SI, I);

		safe_assert(parent_ != NULL);

		count_++;

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_alloca"), FunctionType::get(VOID_TYPE(), false)));
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


// ************* CastInst **************** //

// Callback: void bitcast()
class BitCastInstrumenter : public Instrumenter {
public:
	DEFAULT_CONSTRUCTOR(BitCastInstrumenter);

	bool CheckAndInstrument(Instruction* I) {
		CAST_OR_RETURN(BitCastInst, SI, I);

		safe_assert(parent_ != NULL);

		count_++;

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_bitcast"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

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

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_fpext"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

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

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_fptosi"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

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

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_fptoui"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

		return true;
	}
};


// Callback: void fptoui()
class FPTruncInstrumenter : public Instrumenter {
public:
	DEFAULT_CONSTRUCTOR(FPTruncInstrumenter);

	bool CheckAndInstrument(Instruction* I) {
		CAST_OR_RETURN(FPTruncInst, SI, I);

		safe_assert(parent_ != NULL);

		count_++;

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_fptrunc"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

		return true;
	}
};



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


// ***** TerminatorInst ***** //

// Callback: void branch()
class BranchInstrumenter : public Instrumenter {
public:
  DEFAULT_CONSTRUCTOR(BranchInstrumenter);

	bool CheckAndInstrument(Instruction* I) {
		CAST_OR_RETURN(BranchInst, SI, I);

		safe_assert(parent_ != NULL);

		count_++;

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_branch"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

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

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_indirectbr"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

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

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_resume"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

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

		Instruction *call = CallInst::Create(parent_->M_->getOrInsertFunction(StringRef("llvm_return_"), FunctionType::get(VOID_TYPE(), false)));
		call->insertBefore(I);

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


#endif // INSTRUMENTERS_H_

