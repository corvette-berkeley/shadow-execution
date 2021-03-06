#include "LoadInstrumenter.h"

bool LoadInstrumenter::CheckAndInstrument(Instruction* inst) {
	LoadInst* loadInst = dyn_cast<LoadInst>(inst);

	if (loadInst != NULL) {

		safe_assert(parent_ != NULL);

		count_++;

		Constant* iid = IID_CONSTANT(loadInst);

		Constant* inx = computeIndex(loadInst);

		Constant* loadGlobal = BOOL_CONSTANT(false);

		Constant* loadInx = INT32_CONSTANT(-1, SIGNED);

		if (LoadInst* parentLoadInst =
					dyn_cast<LoadInst>(loadInst->getPointerOperand())) {
			Value* loadPtr = parentLoadInst->getPointerOperand();
			loadGlobal = BOOL_CONSTANT(isa<GlobalVariable>(loadPtr));
			loadInx = computeIndex(loadPtr);
		}

		// debugging info
		string filename = getFileName(loadInst);
		int line = getLineNumber(loadInst);
		int column = getColumnNumber(loadInst);

		DebugInfo* debug = new DebugInfo;
		sprintf(debug->file, "%.99s", filename.c_str());
		debug->line = line;
		debug->column = column;
		IID address = static_cast<IID>(reinterpret_cast<ADDRINT>(loadInst));
		parent_->debugMap[address] = debug;
		// end of debugging info

		if (parent_->fileNames.insert(std::make_pair(filename, parent_->fileCount))
				.second) {
			// element was inserted
			parent_->fileCount++;
		}

		InstrPtrVector instrs;

		Type* type = loadInst->getType();
		if (!type) {
			return false;
		}

		if (type->isStructTy()) {

			// load for structs
			Value* op = KVALUE_VALUE(loadInst->getPointerOperand(), instrs, NOSIGN);
			if (op == NULL) {
				return false;
			}

			KVALUE_STRUCTVALUE(loadInst, instrs);

			while (type->isStructTy()) {
				StructType* structType = (StructType*)type;
				type = structType->getElementType(0);
			}

			KIND kind = TypeToKind(type);
			if (kind == INV_KIND) {
				return false;
			}
			Constant* kindC = KIND_CONSTANT(kind);

			Instruction* call =
				CALL_IID_KIND_KVALUE_INT("llvm_load_struct", iid, kindC, op, inx);
			instrs.push_back(call);

			InsertAllAfter(instrs, loadInst);
		} else {
			// load for non-struct
			Instruction* call;

			Value* ptrOp = loadInst->getPointerOperand();
			Constant* cScope = INT32_CONSTANT(getScope(ptrOp), SIGNED);
			Constant* cOpInx = computeIndex(ptrOp);

			KIND kind = TypeToKind(type);
			if (kind == INV_KIND) {
				return false;
			}
			Constant* kindC = KIND_CONSTANT(kind);

			Instruction* ptrOpCast = PTRTOINT_CAST_INSTR(ptrOp);
			instrs.push_back(ptrOpCast);

			call = CALL_IID_KIND_INT_INT_INT64_BOOL_INT_INT(
					   "llvm_load", iid, kindC, cScope, cOpInx, ptrOpCast, loadGlobal,
					   loadInx, inx);
			instrs.push_back(call);
			InsertAllBefore(instrs, loadInst);
		}
		return true;
	}
	return false;
}

uint64_t LoadInstrumenter::pushStructType(ArrayType* arrayType,
		InstrPtrVector& instrs) {
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

uint64_t LoadInstrumenter::pushStructType(StructType* structType,
		InstrPtrVector& instrs) {
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
