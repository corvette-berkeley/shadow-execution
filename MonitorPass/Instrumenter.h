#ifndef INSTRUMENTER_H_
#define INSTRUMENTER_H_

#include "Common.h"
#include "Instrumentation.h"

/*******************************************************************************************/

typedef std::vector<Type*> 			TypePtrVector;
typedef std::vector<Value*> 		ValuePtrVector;
typedef std::vector<Instruction*>	InstrPtrVector;

/*******************************************************************************************/
// helpful macros for defining instrumenters

#define CAST_OR_RETURN(T, II, I) \
		T* II = dyn_cast<T>(I); \
		if(II == NULL) { \
			return false; \
		}

#define CAST_BINARY_OR_RETURN(T, BI, I, OPC) \
		CAST_OR_RETURN(T, BI, I); \
		if(BI->getOpcode() != Instruction::BinaryOps(Instruction::OPC)) { \
			return false; \
		}

#define DEFAULT_CONSTRUCTOR(T) \
		T(std::string name, Instrumentation* instrumentation) \
		: Instrumenter(name, instrumentation) {}

/*******************************************************************************************/

const bool SIGNED 	= true;
const bool UNSIGNED	= false;
const bool NOSIGN	= false;

/*******************************************************************************************/

class Instrumenter : public InstrumenterBase {
public:
	Instrumenter(std::string name, Instrumentation* instrumentation)
	: InstrumenterBase(name, instrumentation) {}
	virtual ~Instrumenter() {}

protected:

	/*******************************************************************************************/

	inline Type* VOID_TYPE()	{ return Type::getVoidTy(parent_->M_->getContext()); }
	inline Type* INT1_TYPE() 	{ return Type::getInt1Ty(parent_->M_->getContext()); }
	inline Type* INT32_TYPE() 	{ return Type::getInt32Ty(parent_->M_->getContext()); }
	inline Type* INT64_TYPE() 	{ return Type::getInt64Ty(parent_->M_->getContext()); }
	inline Type* PTR64_TYPE()	{ return PointerType::get(VOID_TYPE(), parent_->AS_); }

	inline Type* BOOL_TYPE()	{ return INT1_TYPE(); }

	inline Type* IID_TYPE()		{ return INT64_TYPE(); }
	inline Type* PTR_TYPE()		{ return PTR64_TYPE(); }

	inline Type* FLP32_TYPE()	{ return Type::getFloatTy(parent_->M_->getContext()); }
	inline Type* FLP64_TYPE()	{ return Type::getDoubleTy(parent_->M_->getContext()); }

	inline Type* INTMAX_TYPE()	{ return INT64_TYPE(); }
	inline Type* FLPMAX_TYPE()	{ return FLP64_TYPE(); }

	inline Type* KIND_TYPE()	{ return INT32_TYPE(); }
	inline Type* PRED_TYPE()	{ return INT32_TYPE(); }
	inline Type* VALUE_TYPE()	{ return INTMAX_TYPE(); }

	inline StructType* KVALUE_TYPE()	{
		TypePtrVector typeList;
		typeList.push_back(IID_TYPE());
		typeList.push_back(KIND_TYPE());
		typeList.push_back(VALUE_TYPE());

		return StructType::get(parent_->M_->getContext(), ArrayRef<Type*>(typeList), false /*isPacked*/);
	}
	inline Type* KVALUEPTR_TYPE() 		{ return PointerType::get(KVALUE_TYPE(), parent_->AS_); }

	/*******************************************************************************************/

	inline Constant* INT32_CONSTANT(int32_t c, bool sign)	{ return ConstantInt::get(INT32_TYPE(), c, sign); }
	inline Constant* INT64_CONSTANT(int64_t c, bool sign)	{ return ConstantInt::get(INT64_TYPE(), c, sign); }

	inline Constant* KIND_CONSTANT(KIND c)					{ return ConstantInt::get(KIND_TYPE(), c, UNSIGNED); }
	inline Constant* PRED_CONSTANT(PRED c)					{ return ConstantInt::get(PRED_TYPE(), c, UNSIGNED); }
	inline Constant* INTMAX_CONSTANT(INT c, bool sign)		{ return ConstantInt::get(INTMAX_TYPE(), c, sign); }

	inline Constant* TRUE_CONSTANT()				{ return ConstantInt::getTrue(parent_->M_->getContext()); }
	inline Constant* FALSE_CONSTANT()				{ return ConstantInt::getFalse(parent_->M_->getContext()); }
	inline Constant* BOOL_CONSTANT(bool b)			{ return ((b) ? TRUE_CONSTANT() : FALSE_CONSTANT()); }

	inline Constant* COUNT_CONSTANT(unsigned c)		{ return INT32_CONSTANT(c, UNSIGNED); }
	inline Constant* INDEX_CONSTANT(unsigned c)		{ return INT32_CONSTANT(c, UNSIGNED); }

	// inst is a pointer to an instruction, we cast it to first uintptr_t and then to IID
	inline Constant* IID_CONSTANT(Instruction* inst) { return ConstantInt::get(IID_TYPE(), static_cast<IID>(reinterpret_cast<ADDRINT>(inst)), UNSIGNED); }
	inline Constant* INV_IID_CONSTANT()				 { return ConstantInt::get(IID_TYPE(), INV_IID); }

	/*******************************************************************************************/

	inline Instruction* IID_CAST_INSTR(Value* v)				{ return CastInst::CreateIntegerCast(v, IID_TYPE(), UNSIGNED); }
	inline Instruction* PTR_CAST_INSTR(Value* v)				{ return CastInst::CreatePointerCast(v, PTR_TYPE()); }
	inline Instruction* INTMAX_CAST_INSTR(Value* v, bool sign)	{ return CastInst::CreateIntegerCast(v, INTMAX_TYPE(), sign); }
	inline Instruction* FLPMAX_CAST_INSTR(Value* v)				{ return CastInst::CreateFPCast(v, FLPMAX_TYPE()); }
	inline Instruction* PTRTOINT_CAST_INSTR(Value* v)			{ return new PtrToIntInst(v, INTMAX_TYPE()); }
	inline Instruction* VALUE_CAST_INSTR(Value* v)				{ return BITCAST_INSTR(v, VALUE_TYPE()); }

	/*******************************************************************************************/

	inline Instruction* BITCAST_INSTR(Value* v, Type* T) {
		safe_assert(CastInst::castIsValid(Instruction::CastOps(Instruction::BitCast), v, T));
		return CastInst::CreateTruncOrBitCast(v, T);
	}

	/*******************************************************************************************/

	// allocate (in stack) one instance of given struct type (T), and fill in allocated memory with given elements (vlist)
	Value* AllocateStruct(StructType* T, ValuePtrVector& vlist, InstrPtrVector& Instrs, unsigned align) {
		// allocate memory
		Instruction* I_alloca = new AllocaInst(T, COUNT_CONSTANT(1U), align);
		Instrs.push_back(I_alloca);

		// get prepared for referring to indices in the for loop
		// to refer to an index in the structure a pair of indices is used
		// the first index is always 0, the other points to the index of the element in the structure
		std::vector<Value*> idxList;
		idxList.push_back(INDEX_CONSTANT(0U)); // this is fixed
		idxList.push_back(NULL); // this will change below

		// insert values
		for(unsigned idx = 0; idx < vlist.size(); ++idx) {
			Value* value = vlist[idx];
			idxList[1] = INDEX_CONSTANT(idx);
			Instruction* I_get_addr = GetElementPtrInst::CreateInBounds(I_alloca, ArrayRef<Value*>(idxList));
			Instruction* I_store = new StoreInst(value, I_get_addr, false /*isVolatile*/, align);

			Instrs.push_back(I_get_addr);
			Instrs.push_back(I_store);
		}

		return I_alloca;
	}

	/*******************************************************************************************/

	KIND TypeToKind(Type* T) {
		if(T->isIntegerTy()) {
			switch(T->getScalarSizeInBits()) {
			case 1U:  return INT1_KIND;
			case 8U:  return INT8_KIND;
			case 16U: return INT16_KIND;
			case 32U: return INT32_KIND;
			case 64U: return INT64_KIND;
			}
		} else if(T->isFloatingPointTy()) {
			if(T->isFloatTy()) {
				return FLP32_KIND;
			} else if(T->isDoubleTy()) {
				return FLP64_KIND;
			} else if(T->isFP128Ty()) {
				return FLP128_KIND;
			} else if(T->isX86_FP80Ty()) {
				return FLP80X86_KIND;
			} else if(T->isPPC_FP128Ty()) {
				return FLP128PPC_KIND;;
			}
		} else if(T->isPointerTy()) {
			return PTR_KIND;
		}

		return INV_KIND;
	}

	/*******************************************************************************************/

	Value* KVALUE_VALUE(Value* v, InstrPtrVector& Instrs, bool isSigned) {
		safe_assert(v != NULL);

		// TODO(elmas): what else is OK?
		if(!isa<Instruction>(v) && !isa<Constant>(v)) {
			return NULL;
		}

		Type* T = v->getType();

		KIND kind = TypeToKind(T);
		// if unsupported kind, return NULL
		if(kind == INV_KIND) {
			return NULL;
		}

		Constant* C_iid = NULL;
		Instruction* I_cast = NULL;

		if(isa<Constant>(v)) {
			C_iid = INV_IID_CONSTANT();
		} else { // not constant, but an instruction
			safe_assert(isa<Instruction>(v));
			C_iid = IID_CONSTANT(cast<Instruction>(v));
		}

		if(T->isIntegerTy()) {
			I_cast = INTMAX_CAST_INSTR(v, isSigned);
		} else if(T->isFloatingPointTy()) {
			I_cast = FLPMAX_CAST_INSTR(v);
		} else if(T->isPointerTy()) {
			I_cast = PTRTOINT_CAST_INSTR(v);
		} 

		safe_assert(I_cast != NULL);
		Instrs.push_back(I_cast);

		// bitcast to the value type in the KVALUE struct
		I_cast = VALUE_CAST_INSTR(I_cast);
		Instrs.push_back(I_cast);

		ValuePtrVector fields;
		fields.push_back(C_iid);
		fields.push_back(KIND_CONSTANT(kind));
		fields.push_back(I_cast);

		return AllocateStruct(KVALUE_TYPE(), fields, Instrs, KVALUE_ALIGNMENT);
	}

	/*******************************************************************************************/

	void InsertAllBefore(InstrPtrVector& Instrs, Instruction* I) {
		for(InstrPtrVector::iterator itr = Instrs.begin(); itr < Instrs.end(); ++itr) {
			(*itr)->insertBefore(I);
		}
	}

	void InsertAllAfter(InstrPtrVector& Instrs, Instruction* I) {
		Instruction* I_tmp = I;
		for(InstrPtrVector::iterator itr = Instrs.begin(); itr < Instrs.end(); ++itr) {
			(*itr)->insertAfter(I_tmp);
			I_tmp = (*itr);
		}
	}

	/*******************************************************************************************/

	inline FunctionType* FUNC_TYPE(Type* rtype, TypePtrVector& ptypes)	{ return FunctionType::get(rtype, ArrayRef<Type*>(ptypes), false /*isVarArgs*/); }
	inline FunctionType* VOID_FUNC_TYPE(TypePtrVector& ptypes)			{ return FUNC_TYPE(VOID_TYPE(), ptypes); }

	inline Instruction* CALL_INSTR(const char* fname, FunctionType* ftype, ValuePtrVector& fargs) {
		return CallInst::Create(parent_->M_->getOrInsertFunction(StringRef(fname), ftype), ArrayRef<Value*>(fargs));
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_PTR_KVALUE(const char* func, Value* iid, Value* ptr, Value* kvalue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(PTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(ptr);
		Args.push_back(kvalue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_PTR_KVALUE_KVALUE(const char* func, Value* iid, Value* ptr, Value* kvalue1, Value* kvalue2) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(PTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(ptr);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE(const char* func, Value* iid, Value* b1, Value* kvalue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kvalue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_KVALUE(const char* func, Value* iid, Value* kvalue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kvalue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_KVALUE(const char* func, Value* kvalue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(kvalue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_KVALUE_KVALUE(const char* func, Value* iid, Value* kvalue1, Value* kvalue2) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL(const char* func, Value* iid, Value* b1) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID(const char* func, Value* iid) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_BOOL_KVALUE_KVALUE(const char* func, Value* iid, Value* b1, Value* b2, Value* kvalue1, Value* kvalue2) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(b2);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_KVALUE_KVALUE_PRED(const char* func, Value* iid, Value* k1, Value* k2, Value* p) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(PRED_TYPE());
	  
	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(k1);
	  Args.push_back(k2);
	  Args.push_back(p);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_KVALUE_KVALUE_KVALUE(const char* func, Value* iid, Value* cond, Value* tvalue, Value* fvalue) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  
	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(cond);
	  Args.push_back(tvalue);
	  Args.push_back(fvalue);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_KIND(const char* func, Value* iid, Value* k1) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KIND_TYPE());

	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(k1);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

}; // end Instrumenter

/*******************************************************************************************/

#endif /* INSTRUMENTER_H_ */
