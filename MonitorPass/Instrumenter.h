/**
 * @file Instrumenter.h
 * @brief Instrumenter Declarations.
 */

#ifndef INSTRUMENTER_H_
#define INSTRUMENTER_H_

#include <iostream>
#include "Common.h"
#include "Instrumentation.h"

using namespace std;

typedef std::vector<Type*> 			TypePtrVector;
typedef std::vector<Value*> 		ValuePtrVector;
typedef std::vector<Instruction*>	InstrPtrVector;

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

const bool SIGNED 	= true;
const bool UNSIGNED	= false;
const bool NOSIGN	= false;

class Instrumenter : public InstrumenterBase {
public:
	Instrumenter(std::string name, Instrumentation* instrumentation)
	: InstrumenterBase(name, instrumentation) {}
	virtual ~Instrumenter() {}

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
		typeList.push_back(INT32_TYPE()); // index type
		typeList.push_back(BOOL_TYPE()); // isGlobal
		typeList.push_back(KIND_TYPE());
		typeList.push_back(VALUE_TYPE());

		return StructType::get(parent_->M_->getContext(), ArrayRef<Type*>(typeList), false /*isPacked*/);
	}
	inline Type* KVALUEPTR_TYPE() 		{ return PointerType::get(KVALUE_TYPE(), parent_->AS_); }

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
	inline Constant* IID_CONSTANT(Value* value) { return ConstantInt::get(IID_TYPE(), static_cast<IID>(reinterpret_cast<ADDRINT>(value)), UNSIGNED); }
	inline Constant* INV_IID_CONSTANT()				 { return ConstantInt::get(IID_TYPE(), INV_IID); }

	inline Instruction* IID_CAST_INSTR(Value* v)				{ return CastInst::CreateIntegerCast(v, IID_TYPE(), UNSIGNED); }
	inline Instruction* PTR_CAST_INSTR(Value* v)				{ return CastInst::CreatePointerCast(v, PTR_TYPE()); }
	inline Instruction* INTMAX_CAST_INSTR(Value* v, bool sign)	{ return CastInst::CreateIntegerCast(v, INTMAX_TYPE(), sign); }
	inline Instruction* FLPMAX_CAST_INSTR(Value* v)				{ return CastInst::CreateFPCast(v, FLPMAX_TYPE()); }
	inline Instruction* PTRTOINT_CAST_INSTR(Value* v)			{ return new PtrToIntInst(v, INTMAX_TYPE()); }
	inline Instruction* VALUE_CAST_INSTR(Value* v)				{ return BITCAST_INSTR(v, VALUE_TYPE()); }

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
	    /*
	      if ((dyn_cast<PointerType>(T))->getElementType()->isIntegerTy()) {
	      return INTPTR_KIND;
	      }
	      else {
	      return PTR_KIND;
	      }
	    */
	    return PTR_KIND;
	  } else if (T->isArrayTy()) {
	    return ARRAY_KIND;
	  } else if (T->isStructTy()) {
	    return STRUCT_KIND;
	  } else if (T->isVoidTy()) {
	    return VOID_KIND;
	  }
	  
	  return INV_KIND;
	}
	
	/*******************************************************************************************/

  Constant* computeIndex(Value* value) {
    int inx = -1;
    if (isa<GlobalVariable>(value)) {
      inx = parent_->getGlobalIndex(dyn_cast<GlobalVariable>(value));
    } else if (isa<Constant>(value)) {
      inx = -1;
    } else if (isa<Instruction>(value)) { // not constant, but an instruction
      Instruction* inst = (Instruction*) value;
      inx = parent_->getIndex(inst);
    } else if (isa<BasicBlock>(value)) { // no an instruction, but a basic block
      BasicBlock* block = (BasicBlock*) value;
      inx = parent_->getBlockIndex(block);
    }

    return INT32_CONSTANT(inx, SIGNED);
  }

	/*******************************************************************************************/

  int getLineNumber(Instruction* inst) {
    if (MDNode* node = inst->getMetadata("dbg")) {
      DILocation loc(node);
      return loc.getLineNumber();
    } else {
      return 0;
    }
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
    Constant* C_inx = NULL;
    Constant* C_global = NULL;
    Instruction* I_cast = NULL;

    if (isa<GlobalVariable>(v)) {
      C_iid = IID_CONSTANT(v);
    } else if (isa<Constant>(v)) {
      C_iid = INV_IID_CONSTANT();
    } else { // not constant, but an instruction
      safe_assert(isa<Instruction>(v));
      C_iid = IID_CONSTANT(v);
    }

    C_inx = computeIndex(v);

    bool isGlobal = false;
    if (isa<GlobalVariable>(v)) {
      isGlobal = true;
    }
    C_global = BOOL_CONSTANT(isGlobal);

    if(T->isIntegerTy()) {
      I_cast = INTMAX_CAST_INSTR(v, isSigned);
    } else if(T->isFloatingPointTy()) {
      I_cast = FLPMAX_CAST_INSTR(v);
    } else if(T->isPointerTy()) {
      I_cast = PTRTOINT_CAST_INSTR(v);
    } else {
      printf("Unsupported KVALUE type\n");
      T->dump();
    }

    safe_assert(I_cast != NULL);
    Instrs.push_back(I_cast);

    // bitcast to the value type in the KVALUE struct
    I_cast = VALUE_CAST_INSTR(I_cast);
    Instrs.push_back(I_cast);

    ValuePtrVector fields;
    fields.push_back(C_iid);
    fields.push_back(C_inx);
    fields.push_back(C_global);
    fields.push_back(KIND_CONSTANT(kind));
    fields.push_back(I_cast);

    return AllocateStruct(KVALUE_TYPE(), fields, Instrs, KVALUE_ALIGNMENT);
  }

  /*******************************************************************************************/

  /**
   * Pass a struct value to call backs.
   *
   * @param value struct value to be passed
   * @param instrs accumulation of instructions to be instrumented
   */
  void KVALUE_STRUCTVALUE(Value* value, InstrPtrVector& instrs) {
    safe_assert(value->getType()->isStructTy());
    StructType* structType = (StructType*) value->getType();
    uint64_t size = structType->getNumElements();
    for (uint64_t i = 0 ; i < size; i++) {
      // TODO: use extract value
      std::vector<unsigned> idxList;
      idxList.push_back(i);
      ExtractValueInst* extValInst = ExtractValueInst::Create(value, ArrayRef<unsigned>(idxList));
      instrs.push_back(extValInst);
      Type* elemType = structType->getElementType(i);
      if (elemType->isStructTy()) {
        KVALUE_STRUCTVALUE(extValInst, instrs);
      } else {
        Value* elem = KVALUE_VALUE(extValInst, instrs, NOSIGN);
        if (elem == NULL) safe_assert(false);
        Instruction* call = CALL_KVALUE("llvm_push_return_struct", elem);
        instrs.push_back(call);
      }
    }
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
  Instruction* CALL_IID_PTR_KVALUE_INT(const char* func, Value* iid, Value* ptr, Value* kvalue, Value* inx) {
    TypePtrVector ArgTypes;
    ArgTypes.push_back(IID_TYPE());
    ArgTypes.push_back(PTR_TYPE());
    ArgTypes.push_back(KVALUEPTR_TYPE());
    ArgTypes.push_back(INT32_TYPE());

    ValuePtrVector Args;
    Args.push_back(iid);
		Args.push_back(ptr);
		Args.push_back(kvalue);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_PTR_IID_KVALUE_INT(const char* func, Value* iid, Value* ptr, Value* dest_iid, Value* kvalue, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(PTR_TYPE());
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(ptr);
		Args.push_back(dest_iid);
		Args.push_back(kvalue);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}



	/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_PTR_KVALUE_KVALUE_INT(const char* func, Value* iid, Value* ptr, Value* kvalue1, Value* kvalue2, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(PTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(ptr);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE_INT(const char* func, Value* iid, Value* b1, Value* kvalue, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kvalue);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE_KIND_INT(const char* func, Value* iid, Value* b1, Value* kvalue, Value* kind, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kvalue);
		Args.push_back(kind);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KIND_INT(const char* func, Value* iid, Value* b1, Value* kind, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kind);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}
	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE_KIND_KIND_INT(const char* func, Value* iid, Value* b1, Value* kvalue, Value* kind1, Value* kind2, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kvalue);
		Args.push_back(kind1);
		Args.push_back(kind2);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE_KVALUE_KIND_INT64_INT(const char* func, Value* iid, Value* b1, Value* kvalue, Value* index,
								Value* kind, Value* size, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kvalue);
		Args.push_back(index);
		Args.push_back(kind);
		Args.push_back(size);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE_KVALUE_KIND_INT64_INT_INT(const char* func, Value* iid, Value* b1, Value* kvalue, Value* index,
								Value* kind, Value* size, Value* line, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kvalue);
		Args.push_back(index);
		Args.push_back(kind);
		Args.push_back(size);
		Args.push_back(line);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}
	/*******************************************************************************************/
	Instruction* CALL_IID_KVALUE_INT(const char* func, Value* iid, Value* kvalue, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kvalue);
		Args.push_back(inx);

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
	Instruction* CALL_INT64(const char* func, Value* value) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(value);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_KIND(const char* func, Value* value) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(KIND_TYPE());

		ValuePtrVector Args;
		Args.push_back(value);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_INT64_INT(const char* func, Value* iid, Value* size, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(size);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_KVALUE_INT(const char* func, Value* kvalue, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(kvalue);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_KVALUE_KVALUE_INT(const char* func, Value* iid, Value* kvalue1, Value* kvalue2, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}


	/*******************************************************************************************/
	Instruction* CALL_IID_KVALUE_KVALUE_INT_INT(const char* func, Value* iid, Value* kvalue1, Value* kvalue2, Value* line, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);
		Args.push_back(line);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_INT(const char* func, Value* iid, Value* b1, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_INT_INT(const char* func, Value* inx, Value* inx2) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(inx);
		Args.push_back(inx2);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_INT(const char* func, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_INT(const char* func, Value* iid, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_INT_INT(const char* func, Value* iid, Value* inx, Value* inx2) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(inx);
		Args.push_back(inx2);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_BOOL_KVALUE_KVALUE_INT(const char* func, Value* iid, Value* b1, Value* b2, Value* kvalue1, Value* kvalue2, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(b2);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_KVALUE_KVALUE_PRED_INT(const char* func, Value* iid, Value* k1, Value* k2, Value* p, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(PRED_TYPE());
		ArgTypes.push_back(INT32_TYPE());
	  
	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(k1);
	  Args.push_back(k2);
	  Args.push_back(p);
		Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_KVALUE_KVALUE_KVALUE_INT(const char* func, Value* iid, Value* cond, Value* tvalue, Value* fvalue, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());
	  
	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(cond);
	  Args.push_back(tvalue);
	  Args.push_back(fvalue);
		Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_KIND_INT(const char* func, Value* iid, Value* k1, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());

	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(k1);
		Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	Instruction* CALL_IID_KIND_INT64_INT(const char* func, Value* iid, Value* k1, Value* size, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());

	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(k1);
		Args.push_back(size);
		Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	
	Instruction* CALL_IID_KIND_KVALUE_INT(const char* func, Value* iid, Value* kind, Value* k, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KIND_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(kind);
	  Args.push_back(k);
		Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	
	Instruction* CALL_IID_KIND_KVALUE_INT_INT(const char* func, Value* iid, Value* kind, Value* k, Value* c, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(KIND_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(INT32_TYPE());
	  ArgTypes.push_back(INT32_TYPE());

	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(kind);
	  Args.push_back(k);
	  Args.push_back(c);
	  Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KIND_KVALUE_INT(const char* func, Value* iid, Value* nounwind, Value* kind, Value* k, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(BOOL_TYPE());
	  ArgTypes.push_back(KIND_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());

	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(nounwind);
	  Args.push_back(kind);
	  Args.push_back(k);
		Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

/*******************************************************************************************/

Instruction* CALL_IID_BOOL_KIND_KVALUE_INT_INT(const char* func, Value* iid, Value* nounwind, Value* kind, Value* k, Value* size, Value* inx) {
	  TypePtrVector ArgTypes;
	  ArgTypes.push_back(IID_TYPE());
	  ArgTypes.push_back(BOOL_TYPE());
	  ArgTypes.push_back(KIND_TYPE());
	  ArgTypes.push_back(KVALUEPTR_TYPE());
	  ArgTypes.push_back(INT32_TYPE());
	  ArgTypes.push_back(INT32_TYPE());

	  ValuePtrVector Args;
	  Args.push_back(iid);
	  Args.push_back(nounwind);
	  Args.push_back(kind);
	  Args.push_back(k);
	  Args.push_back(size);
	  Args.push_back(inx);
	  
	  return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}
}; // end Instrumenter

/*******************************************************************************************/

#endif /* INSTRUMENTER_H_ */
