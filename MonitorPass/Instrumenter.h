/**
 * @file Instrumenter.h
 * @brief Instrumenter Declarations.
 */

#ifndef INSTRUMENTER_H_
#define INSTRUMENTER_H_

#include <iostream>
#include <queue>
#include "Common.h"
#include "Instrumentation.h"

using namespace std;

typedef std::vector<Type*> TypePtrVector;
typedef std::vector<Value*> ValuePtrVector;
typedef std::vector<Instruction*> InstrPtrVector;

#define CAST_OR_RETURN(T, II, I)                                                                                       \
	T* II = dyn_cast<T>(I);                                                                                              \
	if (II == NULL) {                                                                                                    \
		return false;                                                                                                      \
	}

#define CAST_BINARY_OR_RETURN(T, BI, I, OPC)                                                                           \
	CAST_OR_RETURN(T, BI, I);                                                                                            \
	if (BI->getOpcode() != Instruction::BinaryOps(Instruction::OPC)) {                                                   \
		return false;                                                                                                      \
	}

const bool SIGNED = true;
const bool UNSIGNED = false;
const bool NOSIGN = false;

class Instrumenter : public InstrumenterBase {
public:
	Instrumenter(std::string name, Instrumentation* instrumentation) : InstrumenterBase(name, instrumentation) {}
	virtual ~Instrumenter() {}

	inline Type* VOID_TYPE() {
		return Type::getVoidTy(parent_->M_->getContext());
	}
	inline Type* INT1_TYPE() {
		return Type::getInt1Ty(parent_->M_->getContext());
	}
	inline Type* INT32_TYPE() {
		return Type::getInt32Ty(parent_->M_->getContext());
	}
	inline Type* INT64_TYPE() {
		return Type::getInt64Ty(parent_->M_->getContext());
	}
	inline Type* PTR64_TYPE() {
		return PointerType::get(VOID_TYPE(), parent_->AS_);
	}

	inline Type* BOOL_TYPE() {
		return INT1_TYPE();
	}

	inline Type* IID_TYPE() {
		return INT64_TYPE();
	}
	inline Type* PTR_TYPE() {
		return PTR64_TYPE();
	}

	inline Type* FLP32_TYPE() {
		return Type::getFloatTy(parent_->M_->getContext());
	}
	inline Type* FLP64_TYPE() {
		return Type::getDoubleTy(parent_->M_->getContext());
	}

	inline Type* INTMAX_TYPE() {
		return INT64_TYPE();
	}
	inline Type* FLPMAX_TYPE() {
		return FLP64_TYPE();
	}

	inline Type* KIND_TYPE() {
		return INT32_TYPE();
	}
	inline Type* PRED_TYPE() {
		return INT32_TYPE();
	}
	inline Type* VALUE_TYPE() {
		return INTMAX_TYPE();
	}

	inline StructType* KVALUE_TYPE() {
		TypePtrVector typeList;
		typeList.push_back(INT32_TYPE());  // index type
		typeList.push_back(BOOL_TYPE());  // isGlobal
		typeList.push_back(KIND_TYPE());
		typeList.push_back(VALUE_TYPE());

		return StructType::get(parent_->M_->getContext(), ArrayRef<Type*>(typeList), false /*isPacked*/);
	}
	inline Type* KVALUEPTR_TYPE() {
		return PointerType::get(KVALUE_TYPE(), parent_->AS_);
	}

	inline Constant* INT32_CONSTANT(int32_t c, bool sign) {
		return ConstantInt::get(INT32_TYPE(), c, sign);
	}
	inline Constant* INT64_CONSTANT(int64_t c, bool sign) {
		return ConstantInt::get(INT64_TYPE(), c, sign);
	}

	inline Constant* KIND_CONSTANT(KIND c) {
		return ConstantInt::get(KIND_TYPE(), c, UNSIGNED);
	}
	inline Constant* PRED_CONSTANT(PRED c) {
		return ConstantInt::get(PRED_TYPE(), c, UNSIGNED);
	}
	inline Constant* INTMAX_CONSTANT(INT c, bool sign) {
		return ConstantInt::get(INTMAX_TYPE(), c, sign);
	}

	inline Constant* TRUE_CONSTANT() {
		return ConstantInt::getTrue(parent_->M_->getContext());
	}
	inline Constant* FALSE_CONSTANT() {
		return ConstantInt::getFalse(parent_->M_->getContext());
	}
	inline Constant* BOOL_CONSTANT(bool b) {
		return ((b) ? TRUE_CONSTANT() : FALSE_CONSTANT());
	}

	inline Constant* COUNT_CONSTANT(unsigned c) {
		return INT32_CONSTANT(c, UNSIGNED);
	}
	inline Constant* INDEX_CONSTANT(unsigned c) {
		return INT32_CONSTANT(c, UNSIGNED);
	}

	// inst is a pointer to an instruction, we cast it to first uintptr_t and then
	// to IID
	inline Constant* IID_CONSTANT(Value* value) {
		return ConstantInt::get(IID_TYPE(), static_cast<IID>(reinterpret_cast<ADDRINT>(value)), UNSIGNED);
	}
	inline Constant* INV_IID_CONSTANT() {
		return ConstantInt::get(IID_TYPE(), INV_IID);
	}

	inline Instruction* IID_CAST_INSTR(Value* v) {
		return CastInst::CreateIntegerCast(v, IID_TYPE(), UNSIGNED);
	}
	inline Instruction* PTR_CAST_INSTR(Value* v) {
		return CastInst::CreatePointerCast(v, PTR_TYPE());
	}
	inline Instruction* INTMAX_CAST_INSTR(Value* v, bool sign) {
		return CastInst::CreateIntegerCast(v, INTMAX_TYPE(), sign);
	}
	inline Instruction* FLPMAX_CAST_INSTR(Value* v) {
		return CastInst::CreateFPCast(v, FLPMAX_TYPE());
	}
	inline Instruction* PTRTOINT_CAST_INSTR(Value* v) {
		return new PtrToIntInst(v, INTMAX_TYPE());
	}
	inline Instruction* VALUE_CAST_INSTR(Value* v) {
		return BITCAST_INSTR(v, VALUE_TYPE());
	}

	inline Instruction* BITCAST_INSTR(Value* v, Type* T) {
		safe_assert(CastInst::castIsValid(Instruction::CastOps(Instruction::BitCast), v, T));
		return CastInst::CreateTruncOrBitCast(v, T);
	}

	/*******************************************************************************************/

	// allocate (in stack) one instance of given struct type (T), and fill in
	// allocated memory with given elements (vlist)
	Value* AllocateStruct(StructType* T, ValuePtrVector& vlist, InstrPtrVector& Instrs, unsigned align) {
		// allocate memory
		Instruction* I_alloca = new AllocaInst(T, COUNT_CONSTANT(1U), align);
		Instrs.push_back(I_alloca);

		// get prepared for referring to indices in the for loop
		// to refer to an index in the structure a pair of indices is used
		// the first index is always 0, the other points to the index of the element
		// in the structure
		std::vector<Value*> idxList;
		idxList.push_back(INDEX_CONSTANT(0U));  // this is fixed
		idxList.push_back(NULL);  // this will change below

		// insert values
		for (unsigned idx = 0; idx < vlist.size(); ++idx) {
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
	int KIND_GetSize(int kind) {
		switch (kind) {
			case PTR_KIND:
				return sizeof(void*);
			case INT8_KIND:
				return 1;
			case INT16_KIND:
				return 2;
			case INT24_KIND:
				return 3;
			case INT32_KIND:
			case FLP32_KIND:
				return 4;
			case INT64_KIND:
			case FLP64_KIND:
				return 8;
			case FLP128_KIND:
				return 16;
			case INT80_KIND:
				return 10;
			case FLP80X86_KIND:
				return 16;
			case FLP128PPC_KIND:
				return 16;
			case INT1_KIND:
				return 0;
			//      return 1;
			//      printf("Don't support bit right now!\n");
			//      safe_assert(false);
			default:
				// shouldn't reach here
				safe_assert(false);
				return 0;
		}
	}

	/*******************************************************************************************/
	KIND TypeToKind(Type* T) {
		if (T->isIntegerTy()) {
			switch (T->getScalarSizeInBits()) {
				case 1U:
					return INT1_KIND;
				case 8U:
					return INT8_KIND;
				case 16U:
					return INT16_KIND;
				case 24U:
					return INT24_KIND;
				case 32U:
					return INT32_KIND;
				case 64U:
					return INT64_KIND;
				case 80U:
					return INT80_KIND;
			}
		} else if (T->isFloatingPointTy()) {
			if (T->isFloatTy()) {
				return FLP32_KIND;
			} else if (T->isDoubleTy()) {
				return FLP64_KIND;
			} else if (T->isFP128Ty()) {
				return FLP128_KIND;
			} else if (T->isX86_FP80Ty()) {
				return FLP80X86_KIND;
			} else if (T->isPPC_FP128Ty()) {
				return FLP128PPC_KIND;
				;
			}
		} else if (T->isPointerTy()) {
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
		} else if (isa<Instruction>(value)) {  // not constant, but an instruction
			Instruction* inst = (Instruction*)value;
			inx = parent_->getIndex(inst);
		} else if (isa<Argument>(value)) {
			Argument* arg = (Argument*)value;
			inx = parent_->getIndex(arg);
		} else if (isa<BasicBlock>(value)) {  // not an instruction, but a basic block
			BasicBlock* block = (BasicBlock*)value;
			inx = parent_->getBlockIndex(block);
		}

		return INT32_CONSTANT(inx, SIGNED);
	}

	SCOPE getScope(Value* value) {
		if (isa<GlobalValue>(value)) {
			return GLOBAL;
		} else if (isa<Constant>(value)) {
			return CONSTANT;
		} else if (isa<GlobalVariable>(value)) {
			return GLOBAL;
		} else {
			safe_assert(isa<Instruction>(value) || isa<Argument>(value));
			return LOCAL;
		}
	}

	Constant* getValueOrIndex(Value* value) {
		Constant* c;

		if (isa<Constant>(value)) {
			if (isa<ConstantInt>(value)) {
				ConstantInt* cInt;
				int64_t v;

				cInt = dyn_cast<ConstantInt>(value);
				v = cInt->getSExtValue();

				c = INT64_CONSTANT(v, SIGNED);
			} else if (isa<ConstantFP>(value)) {
				ConstantFP* cFlp;
				int64_t* ptr;
				int64_t v;
				double dv;

				cFlp = dyn_cast<ConstantFP>(value);
				if (TypeToKind(cFlp->getType()) == FLP32_KIND) {
					dv = cFlp->getValueAPF().convertToFloat();
				} else if (TypeToKind(cFlp->getType()) == FLP64_KIND) {
					dv = cFlp->getValueAPF().convertToDouble();
				} else {
					safe_assert(false);
				}

				ptr = (int64_t*)&dv;
				v = *ptr;

				c = INT64_CONSTANT(v, SIGNED);
			} else if (isa<ConstantPointerNull>(value)) {
				c = INT64_CONSTANT(0, SIGNED);
			} else {
				safe_assert(isa<ConstantExpr>(value));
				c = (Constant*)value;
			}
		} else {
			ConstantInt* cInt;
			int64_t v;

			cInt = (ConstantInt*)computeIndex(value);
			v = cInt->getSExtValue();

			c = INT64_CONSTANT(v, SIGNED);
		}

		return c;
	}
	/*******************************************************************************************/

	int getLineNumber(Instruction* inst) {
		if (MDNode* node = inst->getMetadata("dbg")) {
			DILocation loc(node);
			return loc.getLineNumber();
		}
		return 0;
	}

	int getColumnNumber(Instruction* inst) {
		if (MDNode* node = inst->getMetadata("dbg")) {
			DILocation loc(node);
			return loc.getColumnNumber();
		}
		return 0;
	}

	string getFileName(Instruction* inst) {
		if (MDNode* node = inst->getMetadata("dbg")) {
			DILocation loc(node);
			return loc.getFilename().str();
		}
		return "n/a";
	}

	/*******************************************************************************************/

	Instruction* CAST_VALUE(Value* v, InstrPtrVector& instrs, bool isSigned) {

		Instruction* I_cast = NULL;

		// easier to fix here
		if (v->getType()->isIntegerTy(1)) {
			isSigned = false;
		}

		// TODO(elmas): what else is OK?
		if (!isa<Instruction>(v) && !isa<Constant>(v) && !isa<Argument>(v)) {
			return NULL;
		}

		Type* T = v->getType();

		KIND kind = TypeToKind(T);
		// if unsupported kind, return NULL
		if (kind == INV_KIND) {
			return NULL;
		}

		if (T->isIntegerTy()) {
			I_cast = INTMAX_CAST_INSTR(v, isSigned);
		} else if (T->isFloatingPointTy()) {
			I_cast = FLPMAX_CAST_INSTR(v);
		} else if (T->isPointerTy()) {
			I_cast = PTRTOINT_CAST_INSTR(v);
		} else {
			printf("Unsupported KVALUE type\n");
			T->dump();
		}

		safe_assert(I_cast != NULL);

		// bitcast to VALUE (i64) if needed
		if (!I_cast->getType()->isIntegerTy(64)) {
			instrs.push_back(I_cast);
			I_cast = VALUE_CAST_INSTR(I_cast);
		}
		return I_cast;
	}

	Value* KVALUE_VALUE(Value* v, InstrPtrVector& Instrs, bool isSigned) {
		safe_assert(v != NULL);

		// easier to fix here
		if (v->getType()->isIntegerTy(1)) {
			isSigned = false;
		}

		// TODO(elmas): what else is OK?
		if (!isa<Instruction>(v) && !isa<Constant>(v) && !isa<Argument>(v)) {
			return NULL;
		}

		Type* T = v->getType();

		KIND kind = TypeToKind(T);
		// if unsupported kind, return NULL
		if (kind == INV_KIND) {
			return NULL;
		}

		Constant* C_inx = NULL;
		Constant* C_global = NULL;
		Instruction* I_cast = NULL;

		C_inx = computeIndex(v);

		bool isGlobal = isa<GlobalVariable>(v);
		C_global = BOOL_CONSTANT(isGlobal);

		if (T->isIntegerTy()) {
			I_cast = INTMAX_CAST_INSTR(v, isSigned);
		} else if (T->isFloatingPointTy()) {
			I_cast = FLPMAX_CAST_INSTR(v);
		} else if (T->isPointerTy()) {
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
		StructType* structType = (StructType*)value->getType();
		uint64_t size = structType->getNumElements();
		for (uint64_t i = 0; i < size; i++) {
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
				if (elem == NULL) {
					safe_assert(false);
				}
				Instruction* call = CALL_KVALUE("llvm_push_return_struct", elem);
				instrs.push_back(call);
			}
		}
	}

	/*******************************************************************************************/

	void InsertAllBefore(InstrPtrVector& Instrs, Instruction* I) {
		for (InstrPtrVector::iterator itr = Instrs.begin(); itr < Instrs.end(); ++itr) {
			(*itr)->insertBefore(I);
		}
	}

	void InsertAllAfter(InstrPtrVector& Instrs, Instruction* I) {
		Instruction* I_tmp = I;
		for (InstrPtrVector::iterator itr = Instrs.begin(); itr < Instrs.end(); ++itr) {
			(*itr)->insertAfter(I_tmp);
			I_tmp = (*itr);
		}
	}

	/*******************************************************************************************/

	inline FunctionType* FUNC_TYPE(Type* rtype, TypePtrVector& ptypes) {
		return FunctionType::get(rtype, ArrayRef<Type*>(ptypes), false /*isVarArgs*/);
	}
	inline FunctionType* VOID_FUNC_TYPE(TypePtrVector& ptypes) {
		return FUNC_TYPE(VOID_TYPE(), ptypes);
	}

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
	Instruction* CALL_IID_PTR_IID_KVALUE_INT(const char* func, Value* iid, Value* ptr, Value* dest_iid, Value* kvalue,
			Value* inx) {
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
	Instruction* CALL_IID_PTR_KVALUE_KVALUE_INT(const char* func, Value* iid, Value* ptr, Value* kvalue1, Value* kvalue2,
			Value* inx) {
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
	Instruction* CALL_INT_INT_INT64_INT_INT_INT_INT_INT64_INT64_INT64_INT_INT_INT(
		const char* func, Value* baseInx, Value* baseScope, Value* baseAddr, Value* i0, Value* i1, Value* i2, Value* i3,
		Value* i64_0, Value* i64_1, Value* i64_2, Value* i4, Value* i5, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(baseInx);
		Args.push_back(baseScope);
		Args.push_back(baseAddr);
		Args.push_back(i0);
		Args.push_back(i1);
		Args.push_back(i2);
		Args.push_back(i3);
		Args.push_back(i64_0);
		Args.push_back(i64_1);
		Args.push_back(i64_2);
		Args.push_back(i4);
		Args.push_back(i5);
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
	Instruction* CALL_IID_BOOL_INT_INT_KIND_INT64(const char* func, Value* iid, Value* b1, Value* opInx, Value* opScope,
			Value* opType, Value* opValue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(opInx);
		Args.push_back(opScope);
		Args.push_back(opType);
		Args.push_back(opValue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE_KIND_INT_INT(const char* func, Value* iid, Value* b1, Value* kvalue, Value* kind,
			Value* inx, Value* inx2) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(kvalue);
		Args.push_back(kind);
		Args.push_back(inx);
		Args.push_back(inx2);

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
	Instruction* CALL_IID_BOOL_INT_KIND_INT(const char* func, Value* iid, Value* b1, Value* line, Value* kind,
											Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(line);
		Args.push_back(kind);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_INT_INT_INT64_INT(const char* func, Value* iid, Value* baseInx, Value* baseScope,
											Value* baseAddr, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(baseInx);
		Args.push_back(baseScope);
		Args.push_back(baseAddr);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_BOOL_KVALUE_KVALUE_KIND_INT64_INT(const char* func, Value* iid, Value* b1, Value* kvalue,
			Value* index, Value* kind, Value* size, Value* inx) {
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
	Instruction* CALL_IID_INT_INT_INT64_INT_INT64_KIND_INT64_BOOL_INT_INT(const char* func, Value* iid, Value* baseInx,
			Value* baseScope, Value* baseAddr,
			Value* offsetInx, Value* offsetValue,
			Value* kind, Value* size, Value* loadGlobal,
			Value* loadInx, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(baseInx);
		Args.push_back(baseScope);
		Args.push_back(baseAddr);
		Args.push_back(offsetInx);
		Args.push_back(offsetValue);
		Args.push_back(kind);
		Args.push_back(size);
		Args.push_back(loadGlobal);
		Args.push_back(loadInx);
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
	// here now
	Instruction* CALL_IID_INT_INT_KIND_INT64(const char* func, Value* iid, Value* opInx, Value* opScope, Value* opType,
			Value* opValue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(opInx);
		Args.push_back(opScope);
		Args.push_back(opType);
		Args.push_back(opValue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL(const char* func) {
		TypePtrVector ArgTypes;

		ValuePtrVector Args;

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
	Instruction* CALL_INT_INT_KIND_INT64(const char* func, Value* inx, Value* scope, Value* type, Value* value) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(inx);
		Args.push_back(scope);
		Args.push_back(type);
		Args.push_back(value);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_INT_INT_INT_INT_INT(const char* func, Value* i1, Value* i2, Value* i3, Value* i4, Value* i5) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(i1);
		Args.push_back(i2);
		Args.push_back(i3);
		Args.push_back(i4);
		Args.push_back(i5);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_INT_INT_INT_INT_INT_INT64_INT64_INT64_INT64_INT64(const char* func, Value* i1, Value* i2, Value* i3,
			Value* i4, Value* i5, Value* i64_1, Value* i64_2,
			Value* i64_3, Value* i64_4, Value* i64_5) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(i1);
		Args.push_back(i2);
		Args.push_back(i3);
		Args.push_back(i4);
		Args.push_back(i5);
		Args.push_back(i64_1);
		Args.push_back(i64_2);
		Args.push_back(i64_3);
		Args.push_back(i64_4);
		Args.push_back(i64_5);

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
	Instruction* CALL_INT64_INT_KIND_KIND_INT_INT(const char* func, Value* i64_0, Value* i32_0, Value* kind_0,
			Value* kind_1, Value* i32_1, Value* i32_2) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(i64_0);
		Args.push_back(i32_0);
		Args.push_back(kind_0);
		Args.push_back(kind_1);
		Args.push_back(i32_1);
		Args.push_back(i32_2);

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
	Instruction* CALL_IID_INT64_INT_INT64(const char* func, Value* iid, Value* size, Value* inx, Value* addr) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(size);
		Args.push_back(inx);
		Args.push_back(addr);

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
	Instruction* CALL_IID_KVALUE_KVALUE_INT_INT(const char* func, Value* iid, Value* kvalue1, Value* kvalue2, Value* line,
			Value* inx) {
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
	Instruction* CALL_IID_KVALUE_KVALUE_INT_INT_INT(const char* func, Value* iid, Value* kvalue1, Value* kvalue2,
			Value* file, Value* line, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);
		Args.push_back(file);
		Args.push_back(line);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_KIND_KVALUE_BOOL_INT_INT_INT_INT(const char* func, Value* iid, Value* kind, Value* kvalue1,
			Value* loadGlobal, Value* loadInx, Value* file, Value* line,
			Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kind);
		Args.push_back(kvalue1);
		Args.push_back(loadGlobal);
		Args.push_back(loadInx);
		Args.push_back(file);
		Args.push_back(line);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_KIND_INT_INT_INT64_BOOL_INT_INT(const char* func, Value* iid, Value* kind, Value* scope,
			Value* opInx, Value* opAddr, Value* loadGlobal, Value* loadInx,
			Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(kind);
		Args.push_back(scope);
		Args.push_back(opInx);
		Args.push_back(opAddr);
		Args.push_back(loadGlobal);
		Args.push_back(loadInx);
		Args.push_back(inx);

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
	Instruction* CALL_INT_INT_KIND_INT_INT_INT64(const char* func, Value* i32_0, Value* i32_1, Value* kind, Value* scope,
			Value* srcInx, Value* srcValue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(i32_0);
		Args.push_back(i32_1);
		Args.push_back(kind);
		Args.push_back(scope);
		Args.push_back(srcInx);
		Args.push_back(srcValue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_INT_INT_KVALUE_INT_INT_INT(const char* func, Value* i32_0, Value* i32_1, Value* kv, Value* i32_2,
			Value* i32_3, Value* i32_4) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(i32_0);
		Args.push_back(i32_1);
		Args.push_back(kv);
		Args.push_back(i32_2);
		Args.push_back(i32_3);
		Args.push_back(i32_4);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_INT_INT_INT64_INT64_KIND_PRED_INT(const char* func, Value* i32_1, Value* i32_2, Value* i64_1,
			Value* i64_2, Value* kind, Value* pred, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(PRED_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(i32_1);
		Args.push_back(i32_2);
		Args.push_back(i64_1);
		Args.push_back(i64_2);
		Args.push_back(kind);
		Args.push_back(pred);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_IID_IID_INT_INT_INT64_INT64_KIND_INT(const char* func, Value* iid, Value* lIID, Value* rIID,
			Value* i32_1, Value* i32_2, Value* i64_1, Value* i64_2,
			Value* kind, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(lIID);
		Args.push_back(rIID);
		Args.push_back(i32_1);
		Args.push_back(i32_2);
		Args.push_back(i64_1);
		Args.push_back(i64_2);
		Args.push_back(kind);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_INT_INT_INT64_INT64_KIND_INT(const char* func, Value* iid, Value* i32_1, Value* i32_2,
			Value* i64_1, Value* i64_2, Value* kind, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(i32_1);
		Args.push_back(i32_2);
		Args.push_back(i64_1);
		Args.push_back(i64_2);
		Args.push_back(kind);
		Args.push_back(inx);

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
	Instruction* CALL_IID_BOOL_BOOL_KVALUE_KVALUE_INT_INT(const char* func, Value* iid, Value* b1, Value* b2,
			Value* kvalue1, Value* kvalue2, Value* line, Value* inx) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(b1);
		Args.push_back(b2);
		Args.push_back(kvalue1);
		Args.push_back(kvalue2);
		Args.push_back(line);
		Args.push_back(inx);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	// cindy
	Instruction* CALL_IID_KVALUE_KVALUE_PRED_INT(const char* func, Value* iid, Value* k1, Value* k2, Value* p,
			Value* inx) {
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
	Instruction* CALL_IID_KVALUE_KVALUE_KVALUE_INT(const char* func, Value* iid, Value* cond, Value* tvalue,
			Value* fvalue, Value* inx) {
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
	Instruction* CALL_IID_KIND_INT64_INT_INT64(const char* func, Value* iid, Value* k1, Value* size, Value* inx,
			Value* opValue) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(k1);
		Args.push_back(size);
		Args.push_back(inx);
		Args.push_back(opValue);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/
	Instruction* CALL_IID_KIND_INT64_INT_INT_BOOL(const char* func, Value* iid, Value* k1, Value* size, Value* inx,
			Value* line, Value* arg) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT64_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(BOOL_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(k1);
		Args.push_back(size);
		Args.push_back(inx);
		Args.push_back(line);
		Args.push_back(arg);

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
	Instruction* CALL_IID_BOOL_KIND_KVALUE_INT(const char* func, Value* iid, Value* nounwind, Value* kind, Value* k,
			Value* inx) {
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

	Instruction* CALL_IID_BOOL_KIND_KVALUE_INT_INT(const char* func, Value* iid, Value* nounwind, Value* kind, Value* k,
			Value* size, Value* inx) {
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

	/*******************************************************************************************/  // DELETE!

	Instruction* CALL_IID_BOOL_KIND_KVALUE_INT_INT_KVALUE(const char* func, Value* iid, Value* nounwind, Value* kind,
			Value* k, Value* size, Value* inx, Value* mallocAddress) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(KVALUEPTR_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(nounwind);
		Args.push_back(kind);
		Args.push_back(k);
		Args.push_back(size);
		Args.push_back(inx);
		Args.push_back(mallocAddress);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}

	/*******************************************************************************************/

	Instruction* CALL_IID_BOOL_KIND_INT_INT_INT64(const char* func, Value* iid, Value* nounwind, Value* kind, Value* size,
			Value* inx, Value* mallocAddress) {
		TypePtrVector ArgTypes;
		ArgTypes.push_back(IID_TYPE());
		ArgTypes.push_back(BOOL_TYPE());
		ArgTypes.push_back(KIND_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT32_TYPE());
		ArgTypes.push_back(INT64_TYPE());

		ValuePtrVector Args;
		Args.push_back(iid);
		Args.push_back(nounwind);
		Args.push_back(kind);
		Args.push_back(size);
		Args.push_back(inx);
		Args.push_back(mallocAddress);

		return CALL_INSTR(func, VOID_FUNC_TYPE(ArgTypes), Args);
	}
};  // end Instrumenter

/*******************************************************************************************/

#endif /* INSTRUMENTER_H_ */
