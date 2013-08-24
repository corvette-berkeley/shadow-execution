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

#endif // INSTRUMENTERS_H_

