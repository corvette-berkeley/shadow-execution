/**
 * @file CallInstrumenter.cpp
 * @brief
 */

#include "CallInstrumenter.h"
#include "../src/IValue.h"

bool CallInstrumenter::CheckAndInstrument(Instruction* I) {
	CallInst* callInst = dyn_cast<CallInst>(I);

	if (callInst == NULL) {
		return false;
	}

	bool isIntrinsic = dyn_cast<IntrinsicInst>(callInst) != NULL;
	bool noUnwind = isIntrinsic;

	if (noUnwind && callInst->getType()->isVoidTy()) {
		return false;
	}

	safe_assert(parent_ != NULL);

	count_++;

	InstrPtrVector instrs;
	InstrPtrVector instrsAfter;

	Constant* iid = IID_CONSTANT(callInst);
	Constant* inx = computeIndex(callInst);
	Constant* cLine = INT32_CONSTANT(getLineNumber(callInst), SIGNED);

	Function* callee = callInst->getCalledFunction();

	// for later reference: iid and function name
	/*
	if (callee) {
	  cout << "called function: " << callee->getName().str() << " id: " << endl;
	  iid->dump();
	  cout << endl;
	}
	*/

	// whether this call unwinds the stack
	if (callee != NULL && callee->getName() == "malloc") {
		noUnwind = false;
	}
	Constant* noUnwindC = BOOL_CONSTANT(noUnwind);

	// get return type
	Type* returnType = callInst->getType();
	KIND returnKind = TypeToKind(returnType);
	if (returnKind == INV_KIND) {
		cout << "[CallInstrumenter] => Return type == INV_KIND" << endl;
		safe_assert(false);
	}
	Constant* kind = KIND_CONSTANT(returnKind);

	// get call arguments
	unsigned numArgs = noUnwind ? 0 : callInst->getNumArgOperands();
	unsigned i;

	// push each arguments to the argument stack
	for (i = 0; i < numArgs; i++) {
		// fields to reconstruct KVALUE during interpretation
		Value* arg = callInst->getArgOperand(i);
		Constant* cInx = computeIndex(arg);
		Constant* cScope = INT32_CONSTANT(getScope(arg), NOSIGN);  // original instrumentation was not signed
		Constant* cType = KIND_CONSTANT(TypeToKind(arg->getType()));
		Instruction* argAddress = CAST_VALUE(arg, instrs, NOSIGN);

		if (!argAddress) {
			return false;
		}
		instrs.push_back(argAddress);

		Instruction* call = CALL_INT_INT_KIND_INT64("llvm_push_stack", cInx, cScope, cType, argAddress);
		instrs.push_back(call);
	}

	Instruction* call = NULL;

	// the case for MALLOC
	if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "malloc") {
		// Value* callValue = KVALUE_VALUE(callInst->getCalledValue(), instrs, NOSIGN);

		// there can be more than 1 use
		// e.g., checking for NULL
		// assume bitcast to same type if more than one
		BitCastInst* bitcast = NULL;
		Value::use_iterator it = callInst->use_begin();
		for (; it != callInst->use_end(); it++) {
			if ((bitcast = dyn_cast<BitCastInst>(*it)) != NULL) {
				break;
			}
		}

		Constant* size = NULL;
		if (bitcast != NULL) {

			// PointerType *src = dyn_cast<PointerType>(bitcast->getSrcTy());
			PointerType* dest = dyn_cast<PointerType>(bitcast->getDestTy());

			// cout << "Sizes: " << src->getElementType()->getPrimitiveSizeInBits() << " " <<
			//  dest->getElementType()->getPrimitiveSizeInBits() << endl;

			// cout << "Number of bytes requested: " << endl;
			// callInst->getOperand(0)->dump();

			returnKind = TypeToKind(dest->getElementType());
			if (returnKind == INV_KIND) {
				return false;
			}
			kind = KIND_CONSTANT(returnKind);

			if (TypeToKind(dest->getElementType()) == STRUCT_KIND) {
				uint64_t allocation = pushStructType((StructType*)dest->getElementType(), instrs);
				cout << "After allocation: " << allocation << endl;

				StructType* structType = (StructType*)dest->getElementType();

				// structType->dump();

				unsigned sum = getFlatSize(structType) * 8;

				cout << "Sum is: " << sum;

				safe_assert(sum > 0);

				size = INT32_CONSTANT(sum, false);
			} else if (TypeToKind(dest->getElementType()) == PTR_KIND) {
				size = INT32_CONSTANT(64, false);  // pointers are 64 bits
			} else {
				size = INT32_CONSTANT(dest->getElementType()->getPrimitiveSizeInBits(), false);
			}
			// cout << "Size of struct: ";
			// size->dump();
		} else {
			returnKind = INT8_KIND;
			kind = KIND_CONSTANT(returnKind);

			size = INT32_CONSTANT(8, false);
		}

		// kind is the type of each element (the return type is known to be PTR)
		noUnwind = false;

		// Value* mallocAddress = KVALUE_VALUE(callInst, instrsAfter, NOSIGN);
		// call = CALL_IID_BOOL_KIND_KVALUE_INT_INT_KVALUE("llvm_call_malloc", iid, noUnwindC, kind, callValue, size, inx,
		// mallocAddress);

		Instruction* mallocAddress = CAST_VALUE(callInst, instrsAfter, NOSIGN);

		if (!mallocAddress) {
			return false;
		}
		instrsAfter.push_back(mallocAddress);

		call = CALL_IID_BOOL_KIND_INT_INT_INT64("llvm_call_malloc", iid, noUnwindC, kind, size, inx, mallocAddress);
		instrsAfter.push_back(call);

		InsertAllBefore(instrs, callInst);
		InsertAllAfter(instrsAfter, callInst);
	} else if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "sin") {
		// the case for sin function
		call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_sin", iid, noUnwindC, cLine, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
		return true;
	} else if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "acos") {
		// the case for acos function
		call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_acos", iid, noUnwindC, cLine, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
		return true;
	} else if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "sqrt") {
		// the case for sqrt function
		call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_sqrt", iid, noUnwindC, cLine, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
		return true;
	} else if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "fabs") {
		// the case for fabs function
		call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_fabs", iid, noUnwindC, cLine, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
		return true;
	} else if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "cos") {
		// the case for cos function
		call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_cos", iid, noUnwindC, cLine, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
		return true;
	} else if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "log") {
		// the case for cos function
		call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_log", iid, noUnwindC, cLine, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
		return true;
	}
	/*
	  else if (callInst->getCalledFunction() != NULL &&
	      callInst->getCalledFunction()->getName() == "exp") {
	    //
	    // the case for cos function
	    //
	    call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_exp", iid, noUnwindC, cLine, kind, inx);
	    instrs.push_back(call);
	    InsertAllBefore(instrs, callInst);

	    return true;
	  }
	  */

	else if (callInst->getCalledFunction() != NULL && callInst->getCalledFunction()->getName() == "floor") {
		// the case for cos function
		call = CALL_IID_BOOL_INT_KIND_INT("llvm_call_floor", iid, noUnwindC, cLine, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
		return true;
	} else {
		// the case for general function call
		// kind is the return type of the function
		call = CALL_IID_BOOL_KIND_INT("llvm_call", iid, noUnwindC, kind, inx);
		instrs.push_back(call);
		InsertAllBefore(instrs, callInst);
	}

	if (callInst->getCalledFunction() == NULL || callInst->getCalledFunction()->getName() != "malloc") {
		Instruction* call = NULL;

		if (returnType->isVoidTy()) {
			call = CALL("llvm_after_void_call");
		} else if (returnType->isStructTy()) {
			KVALUE_STRUCTVALUE(callInst, instrsAfter);
			call = CALL("llvm_after_struct_call");
		} else {
			// fields to reconstruct KVALUE during interpretation
			Constant* cInx = computeIndex(callInst);
			Constant* cScope = INT32_CONSTANT(getScope(callInst), NOSIGN);
			Constant* cType = KIND_CONSTANT(TypeToKind(callInst->getType()));
			Instruction* callReturnValue = CAST_VALUE(callInst, instrsAfter, SIGNED);

			if (!callReturnValue) {
				return false;
			}
			instrsAfter.push_back(callReturnValue);

			call = CALL_INT_INT_KIND_INT64("llvm_after_call", cInx, cScope, cType, callReturnValue);
		}
		instrsAfter.push_back(call);
		InsertAllAfter(instrsAfter, callInst);
	}

	return true;
}


uint64_t CallInstrumenter::pushStructType(StructType* structType, InstrPtrVector& instrs) {
	uint64_t size = structType->getNumElements();
	uint64_t allocation = 0;
	for (uint64_t i = 0; i < size; i++) {
		Type* elemType = structType->getElementType(i);
		KIND elemKind = TypeToKind(elemType);
		safe_assert(elemKind != INV_KIND);
		if (elemKind == ARRAY_KIND) {
			allocation += pushStructType((ArrayType*)elemType, instrs);
		} else if (elemKind == STRUCT_KIND) {
			allocation += pushStructType((StructType*)elemType, instrs);
		} else {
			Constant* elemKindC = KIND_CONSTANT(elemKind);
			Instruction* call = CALL_KIND("llvm_push_struct_type", elemKindC);
			instrs.push_back(call);
			allocation++;
		}
	}

	return allocation;
}


uint64_t CallInstrumenter::pushStructType(ArrayType* arrayType, InstrPtrVector& instrs) {
	Type* elemTy = arrayType;
	int size = 1;
	uint64_t allocation = 0;

	while (dyn_cast<ArrayType>(elemTy)) {
		size = size * ((ArrayType*)elemTy)->getNumElements();
		elemTy = ((ArrayType*)elemTy)->getElementType();
	}

	KIND elemKind = TypeToKind(elemTy);
	safe_assert(elemKind != INV_KIND);

	if (elemKind == STRUCT_KIND) {
		for (int i = 0; i < size; i++) {
			allocation += pushStructType((StructType*)elemTy, instrs);
		}
	} else {
		Constant* elemKindC = KIND_CONSTANT(elemKind);

		for (int i = 0; i < size; i++) {
			Instruction* call = CALL_KIND("llvm_push_struct_type", elemKindC);
			instrs.push_back(call);
			allocation++;
		}
	}

	return allocation;
}

unsigned CallInstrumenter::getFlatSize(ArrayType* arrayType) {
	Type* elemTy;
	int size;
	unsigned allocation;
	KIND elemKind;

	//
	// for multi-dimensional array
	// get flatten size
	//
	size = 1;
	elemTy = arrayType;
	while (dyn_cast<ArrayType>(elemTy)) {
		size = size * ((ArrayType*)elemTy)->getNumElements();
		elemTy = ((ArrayType*)elemTy)->getElementType();
	}

	//
	// get element kind
	//
	elemKind = TypeToKind(elemTy);
	safe_assert(elemKind != INV_KIND);

	//
	// element kind can be struct or primitive type
	//
	if (elemKind == STRUCT_KIND) {
		allocation = size * getFlatSize((StructType*)elemTy);
	} else {
		allocation = size * KIND_GetSize(elemKind);
	}

	return allocation;
}

unsigned CallInstrumenter::getFlatSize(StructType* structType) {
	unsigned size, allocation, i;

	allocation = 0;
	size = structType->getNumElements();

	//
	// iterate through all struct element types
	// accumulate allocation size
	//
	for (i = 0; i < size; i++) {
		Type* elemType = structType->getElementType(i);
		KIND elemKind = TypeToKind(elemType);
		safe_assert(elemKind != INV_KIND);

		if (elemKind == ARRAY_KIND) {
			allocation += getFlatSize((ArrayType*)elemType);
		} else if (elemKind == STRUCT_KIND) {
			allocation += getFlatSize((StructType*)elemType);
		} else {
			allocation += KIND_GetSize(elemKind);
		}
	}

	return allocation;
}
