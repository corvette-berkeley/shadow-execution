/**
 * @file GetElementPtrInstrumenter.cpp
 * @brief
 */

#include "GetElementPtrInstrumenter.h"

bool GetElementPtrInstrumenter::CheckAndInstrument(Instruction* inst) {
  GetElementPtrInst* gepInst; 
  InstrPtrVector instrs;
  Constant *iidC, *inxC, *loadInxC, *inbound, *loadGlobal;
  Value *ptrOp;
  PointerType *T;
  Type *elemT;

  gepInst = dyn_cast<GetElementPtrInst>(inst);
  if (gepInst == NULL) {
    return false;
  }

  safe_assert(parent_ != NULL);

  count_++;

  iidC = IID_CONSTANT(gepInst);
  inxC = computeIndex(gepInst);
  loadInxC = INT32_CONSTANT(-1, SIGNED);
  inbound = BOOL_CONSTANT(gepInst->isInBounds());
  loadGlobal = BOOL_CONSTANT(false);
  ptrOp = KVALUE_VALUE(gepInst->getPointerOperand(), instrs, NOSIGN);
  if (ptrOp == NULL) {
    return false;
  }

  if (LoadInst* loadInst = dyn_cast<LoadInst>(gepInst->getPointerOperand())) {
    Value *loadPtr; 

    loadPtr = loadInst->getPointerOperand(); 
    loadGlobal = BOOL_CONSTANT(isa<GlobalVariable>(loadPtr));
    loadInxC = computeIndex(loadPtr);
  }

  T = (PointerType*) gepInst->getPointerOperandType();
  elemT = T->getElementType();

  if (elemT->isArrayTy()) {
    //
    // this branch is the case for local array
    //
    
    Type *gepInstType;
    KIND kind;
    Constant *kindC, *elementSizeC, *scopeInx[3], *valOrInxInx[3], *sizeC[2];
    Instruction *call;
    int elementSize, i;
    queue < Value* > arrIndices;
    queue < int64_t > arrSize;
    
    //
    // Collect all array indices
    //
    for (User::op_iterator idx = gepInst->idx_begin(); idx != gepInst->idx_end(); idx++) {
      arrIndices.push((Value*) idx->get());
    } 

    //
    // Create constants for array indices. Because array is usually up to size
    // 2, getelementptr usually has indices up to size 3. We cover this usual case,
    // and put array indices as arguments to the callback for
    // getelementptr_array. If the array is of dimension greater than two,
    // push_getelementptr_in and push_array_size will be used to push the remain indices
    // and sizes
    //
    for (i = 0; i < 3; i++) {
      if (!arrIndices.empty()) {
        Value *value;
        ActualValue av;

        value = arrIndices.front();
        av = getActualValue(value);
        scopeInx[i] = INT32_CONSTANT(av.scope, SIGNED);
        valOrInxInx[i] = av.valOrInx;
        arrIndices.pop();
      } else {
        scopeInx[i] = INT32_CONSTANT(SCOPE_INVALID, SIGNED);
        valOrInxInx[i] = INT64_CONSTANT(-1, SIGNED);
      }
    }

    //
    // Callbacks for pushing array indices
    //
    while (!arrIndices.empty()){
      Constant *scope[5], *valOrInx[5];
      Instruction *call;
      int i; 

      for (i = 0; i < 5; i++) {
        if (!arrIndices.empty()) {
          Value *value;
          ActualValue av;

          value = arrIndices.front();
          av = getActualValue(value);
          scope[i] = INT32_CONSTANT(av.scope, SIGNED);
          valOrInx[i] = av.valOrInx;
          arrIndices.pop();
        } else {
          scope[i] = INT32_CONSTANT(SCOPE_INVALID, SIGNED);
          valOrInx[i] = INT64_CONSTANT(-1, SIGNED);
        }
      }

      call =
        CALL_INT_INT_INT_INT_INT_INT64_INT64_INT64_INT64_INT64("llvm_push_getelementptr_inx5",
            scope[0], scope[1], scope[2], scope[3], scope[4], valOrInx[0],
            valOrInx[1], valOrInx[2], valOrInx[3], valOrInx[4]);
      instrs.push_back(call);
    } 

    //
    // Collect all array sizes
    //
    while (dyn_cast<ArrayType>(elemT)) {
      arrSize.push(((ArrayType *)elemT)->getNumElements());
      elemT = ((ArrayType *)elemT)->getElementType();
    }

    //
    // Create constants for array sizes. Array is usually up to two dimensions. We cover this usual case,
    // and put array sizes as arguments to the callback for
    // getelementptr_array. If the array is of dimension greater than two,
    // push_getelementptr_in and push_array_size will be used to push the
    // remain indices and sizes
    //
    for (i = 0; i < 2; i++) {
      if (!arrSize.empty()) {
        sizeC[i] = INT32_CONSTANT(arrSize.front(), SIGNED);
        arrSize.pop();
      } else {
        sizeC[i] = INT32_CONSTANT(-1, SIGNED);
      }
    }

    //
    // Callbacks for pushing array sizes
    //
    while (!arrSize.empty()) {
      Constant *size[5];
      Instruction *call;
      int i; 

      for (i = 0; i < 5; i++) {
        if (!arrSize.empty()) {
          size[i] = INT32_CONSTANT(arrSize.front(), SIGNED);
          arrSize.pop();
        } else {
          size[i] = INT32_CONSTANT(-1, SIGNED);
        }
      } 

      call = CALL_INT_INT_INT_INT_INT("llvm_push_array_size5", size[0],
          size[1], size[2], size[3], size[4]);
      instrs.push_back(call);
    }

    gepInstType = ((PointerType*) gepInst->getType())->getElementType();
    kind = TypeToKind(gepInstType);
    kindC = KIND_CONSTANT(kind);

    if (kind == ARRAY_KIND) {
      elementSize = getFlatSize((ArrayType*) gepInstType);
    } else if (kind == STRUCT_KIND) {
      elementSize = getFlatSize((StructType*) gepInstType);
    } else {
      elementSize = KIND_GetSize(kind);
    }

    elementSizeC = INT32_CONSTANT(elementSize, SIGNED);
     
    call = CALL_KVALUE_KIND_INT_INT_INT_INT_INT64_INT64_INT64_INT_INT_INT("llvm_getelementptr_array", ptrOp, kindC, elementSizeC, scopeInx[0], scopeInx[1], scopeInx[2], valOrInxInx[0], valOrInxInx[1], valOrInxInx[2], sizeC[0], sizeC[1], inxC);

    instrs.push_back(call);

  } else if (elemT->isStructTy()) {
    // this branch is the case for local struct
    StructType* structType = (StructType*) elemT;

    // checking struct packing
    //////////////////////////
    if (structType->isPacked()) {
      structType->dump();
      cout << "The struct is packed" << endl;
    }
    else {
      structType->dump();
      cout << "The struct is NOT packed" << endl;
      if (structType->hasName()) {
	cout << "Name: " << structType->getName().str() << endl;
	//abort();
      }
      else {
	cout << "The struct does not have a name" << endl;
	gepInst->dump();
	// do not abort if the struct does not have a name?
	//abort();
      }
    }
    //////////////////////////

    pushStructType(structType, instrs);
    
    for (User::op_iterator idx = gepInst->idx_begin(); idx != gepInst->idx_end(); idx++) {
      Value* idxOp = KVALUE_VALUE(idx->get(), instrs, NOSIGN);
      Instruction* call = CALL_KVALUE("llvm_push_getelementptr_inx", idxOp);
      instrs.push_back(call);
    } 

    // size of struct elements
    pushStructElementSize((StructType*) elemT, instrs);

    Type* gepInstType = ((PointerType*) gepInst->getType())->getElementType();
    KIND kind = TypeToKind(gepInstType);
    Constant* kindC = KIND_CONSTANT(kind);

    Type* elemArrayType = gepInstType; // used only when element is array type

    KIND elemArrayKind = TypeToKind(elemArrayType);
    Constant* elemArrayKindC = KIND_CONSTANT(elemArrayKind);

    Instruction* call = CALL_IID_BOOL_KVALUE_KIND_KIND_INT("llvm_getelementptr_struct", iidC, inbound, ptrOp, kindC, elemArrayKindC, inxC);

    instrs.push_back(call);

  } else {
    // this branch is the case for heap

    if (gepInst->getNumIndices() != 1) {
      cout << "[GetElementPtr] => Multiple indices" << endl;
      abort();
    }

    Value* idxOp = KVALUE_VALUE(gepInst->idx_begin()->get(), instrs, SIGNED);
    if(idxOp == NULL) return false;  

    KIND kind = TypeToKind(elemT);

    Constant* kindC = KIND_CONSTANT(kind);

    Constant* size;
    int isize = 0;

    if (elemT->isPointerTy()) {
      isize = 64;
      size = INT64_CONSTANT(isize, false);
    }
    else {
      isize = elemT->getPrimitiveSizeInBits();
      size = INT64_CONSTANT(isize, false);
    }
    assert(isize != 0);

    Constant* line = INT32_CONSTANT(getLineNumber(gepInst), SIGNED);

    Instruction* call = CALL_IID_BOOL_KVALUE_KVALUE_KIND_INT64_BOOL_INT_INT_INT("llvm_getelementptr", iidC, inbound, ptrOp, idxOp, kindC, size, loadGlobal, loadInxC, line, inxC);
    instrs.push_back(call);

  }

  // instrument
  InsertAllBefore(instrs, gepInst);

  return true;
}

void GetElementPtrInstrumenter::pushStructElementSize(StructType* structType, InstrPtrVector& instrs) {
  uint64_t size, i, allocation;

  size = structType->getNumElements();
  for (i = 0; i < size; i++) {
    Type* elemType = structType->getElementType(i);
    KIND elemKind = TypeToKind(elemType);
    safe_assert(elemKind != INV_KIND);
    if (elemKind == ARRAY_KIND) {
      allocation = getFlatLength((ArrayType*) elemType);
    } else if (elemKind == STRUCT_KIND) {
      allocation = getFlatLength((StructType*) elemType);
    } else {
      allocation = 1;
    }
    Constant* allocC = INT64_CONSTANT(allocation, NOSIGN);
    Instruction* call = CALL_INT64("llvm_push_struct_element_size", allocC);
    instrs.push_back(call);
  }
}

uint64_t GetElementPtrInstrumenter::getFlatSize(ArrayType* arrayType) {
  Type* elemTy; 
  int size; 
  uint64_t allocation;
  KIND elemKind;

  // 
  // for multi-dimensional array
  // get flatten size
  //
  size = 1;
  elemTy = arrayType;
  while (dyn_cast<ArrayType>(elemTy)) {
    size = size*((ArrayType*)elemTy)->getNumElements();
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
    allocation = size * getFlatSize( (StructType*) elemTy );
  } else {
    allocation = size * KIND_GetSize(elemKind); 
  }

  return allocation;
}

uint64_t GetElementPtrInstrumenter::getFlatSize(StructType* structType) {
  uint64_t size, allocation, i;

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
      allocation += getFlatSize((ArrayType*) elemType);
    } else if (elemKind == STRUCT_KIND) {
      allocation += getFlatSize((StructType*) elemType);
    } else {
      allocation += KIND_GetSize(elemKind);
    }
  }

  return allocation;
}

uint64_t GetElementPtrInstrumenter::getFlatLength(ArrayType* arrayType) {
  Type* elemTy; 
  int size; 
  uint64_t allocation;
  KIND elemKind;

  // 
  // for multi-dimensional array
  // get flatten size
  //
  size = 1;
  elemTy = arrayType;
  while (dyn_cast<ArrayType>(elemTy)) {
    size = size*((ArrayType*)elemTy)->getNumElements();
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
    allocation = size* getFlatLength( (StructType*) elemTy );
  } else {
    allocation = size; 
  }

  return allocation;
}

uint64_t GetElementPtrInstrumenter::getFlatLength(StructType* structType) {
  uint64_t size, allocation, i;

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
      allocation += getFlatLength((ArrayType*) elemType);
    } else if (elemKind == STRUCT_KIND) {
      allocation += getFlatLength((StructType*) elemType);
    } else {
      allocation += 1;
    }
  }

  return allocation;
}

uint64_t GetElementPtrInstrumenter::pushStructType(ArrayType* arrayType, InstrPtrVector& instrs) {
  Type* elemTy = arrayType;
  int size = 1;
  uint64_t allocation = 0;

  while (dyn_cast<ArrayType>(elemTy)) {
    size = size*((ArrayType*)elemTy)->getNumElements();
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

uint64_t GetElementPtrInstrumenter::pushStructType(StructType* structType, InstrPtrVector& instrs) {
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

GetElementPtrInstrumenter::ActualValue GetElementPtrInstrumenter::getActualValue(Value *value) {
  SCOPE scope;
  Constant* valOrInx;
  ActualValue av;

  scope = getScope(value);
  valOrInx = getValueOrIndex(value);

  av.scope = scope;
  av.valOrInx = valOrInx;

  return av;
}
