
#include "InterpreterObserver.h"

#include <assert.h>
#include <stack>
#include <vector>
#include <llvm/InstrTypes.h>

void InterpreterObserver::printOffsets(void* addr) {
  for(unsigned i = 0; i < mapOffsets[addr].size(); i++) {
    cout << "i: " << i << "offset: " << mapOffsets[addr][i] << endl;
  }
  return;
}


unsigned InterpreterObserver::findIndex(void* addr, unsigned offset) {
  int low = 0;
  int high = mapOffsets[addr].size() - 1;
  while(low < high){

    int mid = (low + high) / 2;
    cout << "mid: " << mid << endl;
    
    if (offset == mapOffsets[addr][mid]) {
      return mid;
    }
    else if (offset < mapOffsets[addr][mid]) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  return high;
}

void InterpreterObserver::load(IID iid, KVALUE* src, int inx) {
  printf("<<<<< LOAD >>>>> %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*src).c_str(),
	 inx);

  Variable *srcPtrLocation = executionStack.top()[src->inx];

  unsigned srcOffset = srcPtrLocation->getOffset();
  if (srcOffset == 0) {
    Variable *srcLocation = static_cast<Variable*>(srcPtrLocation->getValue().as_ptr);  

    Variable *destLocation = new Variable();
    srcLocation->copy(destLocation);
  
    executionStack.top()[inx] = destLocation;
    cout << destLocation->toString() << endl;
  }
  else {

    void* addr = srcPtrLocation->getValue().as_ptr;
    unsigned objectIndex = findIndex(addr, srcOffset);
    unsigned currOffset = mapOffsets[addr][objectIndex];
    cout << "objectIndex " << objectIndex << " " << srcOffset << " " << currOffset << endl;
    printOffsets(addr);

    if (srcOffset == currOffset) {
      Variable *srcLocation = static_cast<Variable*>((Variable*)(srcPtrLocation->getValue().as_ptr) + objectIndex);  

      Variable *destLocation = new Variable();
      srcLocation->copy(destLocation);
      
      executionStack.top()[inx] = destLocation;
      cout << destLocation->toString() << endl;
    }
    else {
      cout << "[LOAD] => Offset is not zero" << endl;
      abort();
    }
    /*
    Variable *srcLocation = static_cast<Variable*>(srcPtrLocation->getValue().as_ptr);
    
    unsigned ptrCurrSize = srcPtrLocation->getCurrSize();
    unsigned ptrOrigSize = srcPtrLocation->getOrigSize();
    unsigned ptrOffset = srcPtrLocation->getOffset();
    unsigned ptrOffsetSize = srcPtrLocation->getOffsetSize();
    
    unsigned elems =  ((ptrOffset*ptrOffsetSize) + ptrCurrSize) / ptrOrigSize + 1;
    cout << "elems to read: " << elems << endl;

    // retrieving data, we need actual type to distinguish between int/float, etc.
    void* data;
    if (srcPtrLocation->getOrigSize() == 32) {
      data = malloc(sizeof(int)*elems);
      int* idata = (int*)data;
      for(unsigned i = 0; i < elems; i++) {
	idata[i] = srcLocation->getValue().as_int;
	srcLocation++;
      }
    }
    else {
      cerr << "[LOAD] => unhandled type when reading data" << endl;
      safe_assert(false);
    }
    
    // reconstructing data
    if (ptrCurrSize == 16) {
      short* sdata = (short*)data;
      VALUE value;
      value.as_int = sdata[ptrOffset]; /// check!!!!
      Variable* destLocation = new Variable(INT16_KIND, value, false);
      executionStack.top()[inx] = destLocation;
      cout << destLocation->toString() << endl;
    }
    else if (ptrCurrSize == 64) {
      void *newPtr = NULL;
      if (ptrOffsetSize == 16) {
	short* sdata = (short*)data;
	sdata += ptrOffset;
	newPtr = sdata;
      }
      long* ldata = (long*)newPtr;
      VALUE value;
      value.as_int = *ldata;
      Variable* destLocation = new Variable(INT64_KIND, value, false);
      executionStack.top()[inx] = destLocation;
      cout << destLocation->toString() << endl;      
    }
    else {
      cerr << "[LOAD] => unhandled type" << endl;
      safe_assert(false);      
    }
    */

  }
  return;
 }

// ***** Binary Operations ***** //

std::string InterpreterObserver::BINOP_ToString(int binop) {
	std::stringstream s;
	switch(binop) {
    case ADD:
      s << "ADD";
      break;
    case FADD:
      s << "FADD";
      break;
    case SUB:
      s << "SUB";
      break;
    case FSUB:
      s << "FSUB";
      break;
    case MUL:
      s << "MUL";
      break;
    case FMUL:
      s << "FMUL";
      break;
    case UDIV:
      s << "UDIV";
      break;
    case SDIV:
      s << "SDIV";
      break;
    case FDIV:
      s << "FDIV";
      break;
    case UREM:
      s << "UREM";
      break;
    case SREM:
      s << "SREM";
      break;
    case FREM:
      s << "FREM";
      break;
		default: 
      safe_assert(false);
			break;
	}
	return s.str();
}

  long double InterpreterObserver::getValueFromConstant(KVALUE* op) {
    KIND kind = op->kind;
    if (kind == INT1_KIND || kind == INT8_KIND || kind == INT16_KIND || kind == INT32_KIND || kind == INT64_KIND) {
      return op->value.as_int;
    } else {
      return op->value.as_flp;
    }
  }

  long double InterpreterObserver::getValueFromVariable(Variable* loc) {
    KIND kind = loc->getType();
    if (kind == INT1_KIND || kind == INT8_KIND || kind == INT16_KIND || kind == INT32_KIND || kind == INT64_KIND) {
      return loc->getValue().as_int;
    } else {
      return loc->getValue().as_flp;
    }
  }

void InterpreterObserver::binop(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx, BINOP op) {
  printf("<<<<< %s >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", 
      BINOP_ToString(op).c_str(),
      IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(),
      inx);

  long double v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    Variable *loc1 = executionStack.top()[op1->inx];
    v1 = getValueFromVariable(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    Variable *loc2 = executionStack.top()[op2->inx];
    v2 = getValueFromVariable(loc2);
  }

  VALUE vresult;
  switch (op) {
    case ADD:
      vresult.as_int = (int) v1 + (int) v2;
      break;
    case SUB:
      vresult.as_int = (int) v1 - (int) v2;
      break;
    case MUL:
      vresult.as_int = (int) v1 * (int) v2;
      break;
    case UDIV:
      vresult.as_int = (int) v1 / (int) v2;
      break;
    case SDIV:
      vresult.as_int = (int) v1 / (int) v2;
      break;
    case UREM:
      vresult.as_int = (int) v1 % (int) v2;
      break;
    case SREM:
      vresult.as_int = (int) v1 % (int) v2;
      break;
    case FADD:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 + (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 + (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 + v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FSUB:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 - (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 - (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 - v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FMUL:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 * (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 * (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 * v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FDIV:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 / (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 / (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 / v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    default:
      cerr << "[InterpreterObserver::binop] => Unsupported bin op " << BINOP_ToString(op) << "\n";
      abort();
  }

  Variable *nloc = new Variable(op1->kind, vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, ADD);
}

void InterpreterObserver::fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FADD);
}

void InterpreterObserver::sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, SUB);
}

void InterpreterObserver::fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FSUB);
}

void InterpreterObserver::mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, MUL);
}

void InterpreterObserver::fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FMUL);
}

void InterpreterObserver::udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, UDIV);
}

void InterpreterObserver::sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, SDIV);
}

void InterpreterObserver::fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FDIV);
}

void InterpreterObserver::urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, UREM);
}

void InterpreterObserver::srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, SREM);
}

void InterpreterObserver::frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< FREM >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  cerr << "[InterpreterObserver::extractvalue] => Unsupported in C???\n";
  abort();
}


// ***** Bitwise Binary Operations ***** //
void InterpreterObserver::shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Variable *loc1 = executionStack.top()[op1->inx];
  Variable *loc2 = executionStack.top()[op2->inx];
  int result = loc1->getValue().as_int << loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(loc1->getType(), vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Variable *loc1 = executionStack.top()[op1->inx];
  Variable *loc2 = executionStack.top()[op2->inx];
  int result = loc1->getValue().as_int >> loc2->getValue().as_int; // same as arith?

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(loc1->getType(), vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Variable *loc1 = executionStack.top()[op1->inx];
  Variable *loc2 = executionStack.top()[op2->inx];
  int result = loc1->getValue().as_int >> loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(loc1->getType(), vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Variable *loc1 = executionStack.top()[op1->inx];
  Variable *loc2 = executionStack.top()[op2->inx];
  int result = loc1->getValue().as_int & loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(loc1->getType(), vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Variable *loc1 = executionStack.top()[op1->inx];
  Variable *loc2 = executionStack.top()[op2->inx];
  int result = loc1->getValue().as_int | loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(loc1->getType(), vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Variable *loc1 = executionStack.top()[op1->inx];
  Variable *loc2 = executionStack.top()[op2->inx];
  int result = loc1->getValue().as_int ^ loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(loc1->getType(), vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

// ***** Vector Operations ***** //

void InterpreterObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< EXTRACTELEMENT >>>>> %s, vector:%s, index:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  cerr << "[InterpreterObserver::extractelement] => Unimplemented\n";
  abort();
}

void InterpreterObserver::insertelement() {
  printf("<<<<< INSERTELEMENT >>>>>\n");

  cerr << "[InterpreterObserver::insertelement] => Unimplemented\n";
  abort();
}

void InterpreterObserver::shufflevector() {
  printf("<<<<< SHUFFLEVECTOR >>>>>\n");

  cerr << "[InterpreterObserver::shufflevector] => Unimplemented\n";
  abort();
}


// ***** AGGREGATE OPERATIONS ***** //

void InterpreterObserver::extractvalue(IID iid, KVALUE* op, int inx) {
  printf("<<<<< EXTRACTVALUE >>>>> %s, agg_val:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op).c_str(), 
      inx);

  cerr << "[InterpreterObserver::extractvalue] => Unimplemented\n";
  abort();
}

void InterpreterObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< INSERTVALUE >>>>> %s, vector:%s, value:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  cerr << "[InterpreterObserver::insertvalue] => Unimplemented\n";
  abort();
}

// ***** Memory Access and Addressing Operations ***** //

void InterpreterObserver::allocax(IID iid, KIND type, uint64_t size, int inx) {
  printf("<<<<< ALLOCA >>>>> %s, kind:%s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(type).c_str(), size, inx);

  Variable* ptrLocation;
  if (callArgs.empty()) {
    Variable *location = new Variable(type, false);
    VALUE value;
    value.as_ptr = location;
    ptrLocation = new Variable(PTR_KIND, value, true);
    executionStack.top()[inx] = ptrLocation;
  } else {
    Variable *location = callArgs.top();
    VALUE value;
    value.as_ptr = (void*) location;
    ptrLocation = new Variable(PTR_KIND, value, true);
    executionStack.top()[inx] = ptrLocation;
    callArgs.pop();
  }

  cout << ptrLocation->toString() << "\n";

  return;
}

void InterpreterObserver::allocax_array(IID iid, KIND type, uint64_t size, int inx) {
  printf("<<<<< ALLOCA_ARRAY >>>>> %s, elemkind:%s, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(type).c_str(), inx);

  size = 1;
  while (!arraySize.empty()) {
    size = size * arraySize.front();
    arraySize.pop();
  }

  uint64_t structSize = 1;
  if (type == STRUCT_KIND) structSize = structType.size();
  KIND* structKind = (KIND*) malloc(structSize*sizeof(KIND));
  for (uint64_t i = 0; i < structSize; i++) {
    structKind[i] = structType.front();
    structType.pop();
  }

  if (callArgs.empty()) {
    Variable** locArr = (Variable**) malloc(size*structSize*sizeof(Variable*));
    for (uint64_t i = 0; i < size; i++) {
      if (type == STRUCT_KIND) {
        for (uint64_t j = 0; j < structSize; j++) {
          Variable* var = new Variable(structKind[j], false);
          locArr[i*structSize+j] = var;
        }
      } else {
        Variable* var = new Variable(type, false); 
        locArr[i] = var;
      }
    }

    VALUE locArrPtrVal;
    locArrPtrVal.as_ptr = (void*) locArr; 
    Variable* locArrPtr = new Variable(PTR_KIND, locArrPtrVal, true);
    executionStack.top()[inx] = locArrPtr;
  } else {
    Variable *location = callArgs.top();
    VALUE value;
    value.as_ptr = (void*) location;
    Variable* ptrLocation = new Variable(PTR_KIND, value, true); 
    executionStack.top()[inx] = ptrLocation;
    callArgs.pop();
  }

  cout << executionStack.top()[inx]->toString() << "\n";

  return;
}

void InterpreterObserver::allocax_struct(IID iid, uint64_t size, int inx) {
  printf("<<<<< ALLOCA STRUCT >>>>> %s, size: %ld, [INX: %d]\n", IID_ToString(iid).c_str(), size, inx);

  if (callArgs.empty()) {
    Variable** ptrToStructVar = (Variable**) malloc(size*sizeof(Variable*));
    for (uint64_t i = 0; i < size; i++) {
      KIND type = structType.front();
      Variable* var = new Variable(type, false);
      ptrToStructVar[i] = var;
      structType.pop();
    }
    safe_assert(structType.empty());

    VALUE structPtrVal;
    structPtrVal.as_ptr = (void*) ptrToStructVar;
    Variable* structPtrVar = new Variable(PTR_KIND, structPtrVal, false);

    executionStack.top()[inx] = structPtrVar;
  } else {
    Variable *location = callArgs.top();
    VALUE value;
    value.as_ptr = (void*) location;
    Variable* ptrLocation = new Variable(PTR_KIND, value, true); 
    executionStack.top()[inx] = ptrLocation;
    callArgs.pop();
  }

  cout << executionStack.top()[inx]->toString() << "\n";
}

bool checkStore(Variable *dest, KVALUE *kv) {
  bool result = false;

  switch(kv->kind) {
    case PTR_KIND:
      result = true; // don't compare pointer
      // result = (dest->getValue().as_ptr == kv->value.as_ptr);
      break;
    case INT1_KIND: 
    case INT8_KIND: 
    case INT16_KIND: 
    case INT32_KIND: 
    case INT64_KIND:
      result = (dest->getValue().as_int == kv->value.as_int);
      break;
    case FLP32_KIND:
      result = ((float)dest->getValue().as_flp) == ((float)kv->value.as_flp);
      break;
    case FLP64_KIND:
      result = ((double)dest->getValue().as_flp) == ((double)kv->value.as_flp);
      break;
    case FLP80X86_KIND:
      result = dest->getValue().as_flp == kv->value.as_flp;
      break;
    default: //safe_assert(false);
      break;
  }

  return result;
}

void InterpreterObserver::store(IID iid, KVALUE* dest, KVALUE* src, int inx) {
  printf("<<<<< STORE >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*dest).c_str(), //op
	 KVALUE_ToString(*src).c_str(), inx); // kv

  // retrieve Variable to store in
  Variable *destPtrLocation = executionStack.top()[dest->inx];
  unsigned destPtrOffset = destPtrLocation->getOffset();

  if (destPtrOffset == 0) {
    Variable *destLocation = static_cast<Variable*>(destPtrLocation->getValue().as_ptr);
    //destLocation += destPtrLocation->getOffset();
    cout << "Dest: " << destLocation->toString() << endl;

    // the value to store is a constant
    if (src->iid == 0) {
      destLocation->setValue(src->value);
    }
    else {
      Variable* srcLocation = executionStack.top()[src->inx];
      srcLocation->copy(destLocation);
      cout << "Src: " << srcLocation->toString() << endl;
    }
    cout << "Updated Dest: " << destLocation->toString() << endl;
    
    if (!checkStore(destLocation, src)) {
      cerr << "KVALUE: " << KVALUE_ToString(*src) << endl;
      cerr << "Mismatched values found in Store" << endl;
      abort();
    }
  }
  else {

    void* addr = destPtrLocation->getValue().as_ptr;
    cout << "destPtrOffset: " << destPtrOffset << endl;
    unsigned objectIndex = findIndex(addr, destPtrOffset);
    unsigned currOffset = mapOffsets[addr][objectIndex];
    cout << "objectIndex: " << objectIndex << " currOffset: " << currOffset << endl;

    if (destPtrOffset == currOffset) {
      Variable *destLocation = ((Variable*)destPtrLocation->getValue().as_ptr) + objectIndex;
      cout << "Dest: " << destLocation->toString() << endl;

      Variable* srcLocation = NULL;
      // the value to store is a constant
      if (src->iid == 0) {
	srcLocation = new Variable(INT64_KIND, src->value, false);
      }
      else {
	srcLocation = executionStack.top()[src->inx];
      }
      cout << "Src: " << srcLocation->toString() << endl;
      srcLocation->copy(destLocation);

      cout << "Updated Dest: " << destLocation->toString() << endl;
      
      if (!checkStore(destLocation, src)) {
	cerr << "KVALUE: " << KVALUE_ToString(*src) << endl;
	cerr << "Mismatched values found in Store" << endl;
	abort();
      }
    }
    else {
      cout << "[STORE] => Offset is not zero" << endl;
    }

  /*
    Variable *destLocation = static_cast<Variable*>(destPtrLocation->getValue().as_ptr);
    cout << "Dest: " << destLocation->toString() << endl;

    unsigned ptrCurrSize = destPtrLocation->getCurrSize();
    unsigned ptrOrigSize = destPtrLocation->getOrigSize();
    unsigned ptrOffset = destPtrLocation->getOffset();
    unsigned ptrOffsetSize = destPtrLocation->getOffsetSize();
    
    unsigned elems =  ((ptrOffset*ptrOffsetSize) + ptrCurrSize) / ptrOrigSize + 1; // ????
    cout << "elems to write: " << elems << endl;

    // data to be written
    Variable* srcLocation = NULL;
    // the value to store is a constant
    if (src->iid == 0) {
      srcLocation = new Variable(INT64_KIND, src->value, false);
    }
    else {
      srcLocation = executionStack.top()[src->inx];
    }
    cout << "Src: " << srcLocation->toString() << endl;

    if (srcLocation->getType() == INT64_KIND) {
      long *data = (long*)malloc(sizeof(long));
      *data = srcLocation->getValue().as_int;
      cout << "data to write: " << *data << endl;

      void *vdata = NULL;
      if (ptrOffsetSize == 16) {
	short* sdata = (short*)data;
	//sdata += (ptrOffset - 1);
	
	// first element has to be split and updated
	int val = destLocation->getValue().as_int;
	short* sdataOrig = (short*)&val;
	sdataOrig[1] = *sdata;
	int* idata = (int*)sdataOrig;
	VALUE value;
	value.as_int = *idata;
	destLocation->setValue(value);
	vdata = sdata++;
	cout << destLocation->toString() << endl;
      }

      int* idata = (int*)vdata;
      for(unsigned i = 1; i < elems - 1; i++ ) {
	VALUE value;
	value.as_int = idata[i];
	destLocation++;
	destLocation->setValue(value);
	executionStack.top()[inx] = destLocation; // no need for this...
	cout << destLocation->toString() << endl;

	// need to check each store
      }

      // last update first half
      short* sdata = (short*)(&idata[elems-1]);
      destLocation++;
      cout << "last element before updating: " << destLocation->toString() << endl;

      int val = destLocation->getValue().as_int;
      short* sdataOrig = (short*)&val;
      sdataOrig[0] = sdata[0];
      idata = (int*)sdataOrig;
      VALUE value;
      value.as_int = *idata;
      destLocation->setValue(value);
      cout << destLocation->toString() << endl;

    }
    else {
      cout << "[STORE] => unhandled type of data to write" << endl;
    }
    */
  }
  return;
}

void InterpreterObserver::fence() {
  printf("<<<<< FENCE >>>>>\n");

  cerr << "[InterpreterObserver::fence] => Unimplemented\n";
  abort();
}

void InterpreterObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx) {
  printf("<<<<< CMPXCHG >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      PTR_ToString(addr).c_str(),
      KVALUE_ToString(*kv1).c_str(),
      KVALUE_ToString(*kv2).c_str(), 
      inx);

  cerr << "[InterpreterObserver::cmpxchg] => Unimplemented\n";
  abort();
}

void InterpreterObserver::atomicrmw() {
  printf("<<<<< ATOMICRMW >>>>>\n");

  cerr << "[InterpreterObserver::atomicrmw] => Unimplemented\n";
  abort();
}

void InterpreterObserver::getelementptr(IID iid, bool inbound, KVALUE* base, KVALUE* offset, KIND type, uint64_t size, int inx) {
  printf("<<<<< GETELEMENTPTR >>>>> %s, inbound:%s, pointer_value:%s, index:%s, kind: %s, size: %ld, [INX: %d]\n", 
      IID_ToString(iid).c_str(),
      (inbound ? "1" : "0"),
      KVALUE_ToString(*base).c_str(),
      KVALUE_ToString(*offset).c_str(),
      KIND_ToString(type).c_str(),
      size,
      inx);
  
  Variable* basePtrLocation = executionStack.top()[base->inx];

  int offsetValue;
  if (offset->inx != -1) {
    offsetValue = executionStack.top()[offset->inx]->getValue().as_int;
  }
  else {
    offsetValue = offset->value.as_int;
  }

  // offset in bytes from base ptr
  unsigned newOffset = (offsetValue * (size/8)) + basePtrLocation->getOffset();
  Variable* ptrLocation = new Variable(PTR_KIND, basePtrLocation->getValue(), size/8, newOffset, false);
  
  executionStack.top()[inx] = ptrLocation;
  cout << executionStack.top()[inx]->toString() << "\n";
  
  return;
}

void InterpreterObserver::getelementptr_array(IID iid, bool inbound, KVALUE* op, KIND kind, int inx) {
  printf("<<<<< GETELEMENTPTR_ARRAY >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, [INX: %d]\n", 
      IID_ToString(iid).c_str(),
      (inbound ? "1" : "0"),
      KVALUE_ToString(*op).c_str(),
      KIND_ToString(kind).c_str(),
      inx);

  Variable** array = static_cast<Variable**>(executionStack.top()[op->inx]->getValue().as_ptr);
  getElementPtrIndexList.pop();

  int size = 1;
  arraySize.pop();
  while (!arraySize.empty()) {
    size = size * arraySize.front();
    arraySize.pop();
  }
  size = getElementPtrIndexList.front()*size;
  getElementPtrIndexList.pop();

  cout << "Getting element at index : " << size << "\n";
  if (kind == ARRAY_KIND || kind == STRUCT_KIND ) {
    Variable** subArray = array + size;
    VALUE subArrVal;
    subArrVal.as_ptr = (void*) subArray;
    Variable* subArrPtrVar = new Variable(PTR_KIND, subArrVal, false);
    executionStack.top()[inx] = subArrPtrVar;
  } else {
    Variable* arrayElem = array[size];
    VALUE arrayElemAddrVal;
    arrayElemAddrVal.as_ptr = (void*) arrayElem;
    Variable* arrayElemPtr = new Variable(PTR_KIND, arrayElemAddrVal, false);
    executionStack.top()[inx] = arrayElemPtr;
  }

  cout << executionStack.top()[inx]->toString() << "\n";
}

void InterpreterObserver::getelementptr_struct(IID iid, bool inbound, KVALUE* op, KIND kind, KIND arrayKind, int inx) {
    printf("<<<<< GETELEMENTPTR_STRUCT >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, arrayKind: %s, [INX: %d]\n", 
      IID_ToString(iid).c_str(),
      (inbound ? "1" : "0"),
      KVALUE_ToString(*op).c_str(),
      KIND_ToString(kind).c_str(),
      KIND_ToString(arrayKind).c_str(),
      inx);

    Variable** structVar = static_cast<Variable**>(executionStack.top()[op->inx]->getValue().as_ptr);

    int index;
    getElementPtrIndexList.pop();
    index = getElementPtrIndexList.front();
    getElementPtrIndexList.pop();

    Variable* structElemPtr;

    if (kind == ARRAY_KIND || kind == STRUCT_KIND) {
      Variable** subElem = structVar + index;
      VALUE structElemPtrVal;
      structElemPtrVal.as_ptr = (void*) subElem;
      structElemPtr = new Variable(PTR_KIND, structElemPtrVal, false);
    } else {
      Variable* structElem = structVar[index];
      VALUE structElemPtrVal;
      structElemPtrVal.as_ptr = (void*) structElem;
      structElemPtr = new Variable(PTR_KIND, structElemPtrVal, false);
    }

    executionStack.top()[inx] = structElemPtr;

    cout << executionStack.top()[inx]->toString() << "\n";
}

// ***** Conversion Operations ***** //

void InterpreterObserver::trunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  printf("<<<<< TRUNC >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), size, inx);

  cerr << "[InterpreterObserver::trunc] => Unimplemented\n";
  abort();
}

void InterpreterObserver::zext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  printf("<<<<< ZEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), size, inx);

  Variable *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();
  int64_t intValue = value.as_int;
  VALUE zextValue;

  switch (type) {
    case INT1_KIND:
      zextValue.as_int = intValue & (1<<0); // TODO: confirm this
      break;
    case INT8_KIND:
      zextValue.as_int = (int8_t) intValue;
      break;
    case INT16_KIND:
      zextValue.as_int = (int16_t) intValue;
      break;
    case INT32_KIND: 
      zextValue.as_int = (int32_t) intValue;
      break;
    case INT64_KIND:
      zextValue.as_int = (int64_t) intValue;
      break;
    default:
      cerr << "[InterpreterObserver::zext] => Unsupport integer type " << type << "\n";
      safe_assert(false);
  }

  Variable* zextVar = new Variable(type, zextValue, false);
  executionStack.top()[inx] = zextVar;
  cout << executionStack.top()[inx]->toString() << "\n";
}

void InterpreterObserver::sext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  printf("<<<<< SEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), size, inx);

  /*
  cerr << "[InterpreterObserver::sext] => Unimplemented.\n";
  abort();
  */
  Variable *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();

  VALUE ext_value;

  switch (type) {
    case INT1_KIND:
      ext_value.as_int = value.as_int & (1<<0); // TODO: confirm this
      break;
    case INT8_KIND:
      ext_value.as_int = (int8_t) value.as_int;
      break;
    case INT16_KIND:
      ext_value.as_int = (int16_t) value.as_int;
      break;
    case INT32_KIND: 
      ext_value.as_int = (int32_t) value.as_int;
      break;
    case INT64_KIND:
      ext_value.as_int = (int64_t) value.as_int;
      break;
    default:
      cerr << "[InterpreterObserver::sext] => Unsupport integer type " << type << "\n";
      abort();
  }

  Variable *ext_loc = new Variable(type, ext_value, false);
  executionStack.top()[inx] = ext_loc;
  cout << ext_loc->toString() << "\n";
}

void InterpreterObserver::fptrunc(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  printf("<<<<< FPTRUNC >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), size, inx);

  Variable *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_flp;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_flp;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_flp;
  }
  else 
  {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Variable *trunc_loc = new Variable(type, trunc_value, false);
  executionStack.top()[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fpext(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  printf("<<<<< FPEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), size, inx);

  Variable *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_flp;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_flp;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_flp;
  }
  else 
  {
    cerr << "[InterpreterObserver::fpext] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Variable *trunc_loc = new Variable(type, trunc_value, false);
  executionStack.top()[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fptoui(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  printf("<<<<< FPTOUII >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), size, inx);

  Variable *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  Variable *trunc_loc = new Variable(type, trunc_value, false);
  executionStack.top()[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fptosi(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  printf("<<<<< FPTOSI >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), size, inx);

  Variable *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  Variable *trunc_loc = new Variable(type, trunc_value, false);
  executionStack.top()[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::uitofp(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  printf("<<<<< UITOFP >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), size, inx);

  Variable *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_int;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_int;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_int;
  }
  else 
  {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Variable *trunc_loc = new Variable(type, trunc_value, false);
  executionStack.top()[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::sitofp(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  printf("<<<<< SITOFP >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), size, inx);

  Variable *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_int;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_int;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_int;
  }
  else 
  {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Variable *trunc_loc = new Variable(type, trunc_value, false);
  executionStack.top()[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";

}

void InterpreterObserver::ptrtoint(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  printf("<<<<< PTRTOINT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), size, inx);

  Variable *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();
  int64_t ptrValue = value.as_int;

  VALUE int_value;

  switch (type) {
    case INT1_KIND:
      int_value.as_int = ptrValue & (1<<0); // TODO: confirm this
      break;
    case INT8_KIND:
      int_value.as_int = (int8_t) ptrValue;
      break;
    case INT16_KIND:
      int_value.as_int = (int16_t) ptrValue;
      break;
    case INT32_KIND:
      int_value.as_int = (int32_t) ptrValue;
      break;
    case INT64_KIND:
      int_value.as_int = ptrValue;
      break;
    default:
      safe_assert(false); // this cannot happen
  }

  Variable *ptrToInt = new Variable(type, int_value, false);
  executionStack.top()[inx] = ptrToInt;
  cout << executionStack.top()[inx]->toString() << "\n";
}

void InterpreterObserver::inttoptr(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  printf("<<<<< INTTOPTR >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), size, inx);

  Variable *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();
  VALUE ptr_value;
  ptr_value.as_ptr = value.as_ptr;
  Variable *intToPtr = new Variable(type, ptr_value, false);
  executionStack.top()[inx] = intToPtr;
  cout << executionStack.top()[inx]->toString() << "\n";
}

void InterpreterObserver::bitcast(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  printf("<<<<< BITCAST >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), size, inx);

  Variable *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();

  Variable *bitcastLoc = new Variable(type, value, size/8, src->getOffset(), false); // TODO: check
  executionStack.top()[inx] = bitcastLoc;
  cout << bitcastLoc->toString() << endl;
  return;
}


// ***** TerminatorInst ***** //
void InterpreterObserver::branch(IID iid, bool conditional, KVALUE* op1, int inx) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (conditional ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(), inx);
}

void InterpreterObserver::branch2(IID iid, bool conditional, int inx) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (conditional ? "1" : "0"), inx);
}

void InterpreterObserver::indirectbr(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< INDIRECTBR >>>>> %s, address: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(), inx);
}

void InterpreterObserver::invoke(IID iid, KVALUE* call_value, int inx) {
  printf("<<<<< INVOKE >>>>> %s, call_value: %s, [INX: %d]", IID_ToString(iid).c_str(), KVALUE_ToString(*call_value).c_str(), inx);
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    printf(", arg: %s", KVALUE_ToString(*value).c_str()); 
  }
  printf("\n");

  cerr << "[InterpreterObserver::invoke] => Unimplemented\n";
  abort();
}

void InterpreterObserver::resume(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< RESUME >>>>> %s, acc_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(), inx);

  cerr << "[InterpreterObserver::resume] => Unimplemented\n";
  abort();
}

void InterpreterObserver::return_(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< RETURN >>>>> %s, ret_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(), inx);

  safe_assert(!executionStack.empty());
  executionStack.pop();

  if (!executionStack.empty()) {
    cout << "New stack size: " << executionStack.size() << "\n";
    if (!callerVarIndex.empty()) {
      executionStack.top()[callerVarIndex.top()]->setValue(op1->value); 
      executionStack.top()[callerVarIndex.top()]->setType(op1->kind); 
      cout << executionStack.top()[callerVarIndex.top()]->toString() << "\n";

      safe_assert(!callerVarIndex.empty());
      callerVarIndex.pop();
    }
  } else {
    cout << "The execution stack is empty.\n";
  }

  return;
}

void InterpreterObserver::return2_(IID iid, int inx) {
  printf("<<<<< RETURN >>>>> %s, [INX: %d]\n", IID_ToString(iid).c_str(), inx);

  safe_assert(!executionStack.empty());
  executionStack.pop();

  if (!executionStack.empty()) {
    cout << "New stack size: " << executionStack.size() << "\n";
  } else {
    cout << "The execution stack is empty.\n";
  }

  safe_assert(!callerVarIndex.empty());
  callerVarIndex.pop();

  return;
}

void InterpreterObserver::switch_(IID iid, KVALUE* op, int inx) {
  printf("<<<<< SWITCH >>>>> %s, condition: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op).c_str(), inx);
}

void InterpreterObserver::unreachable() {
  printf("<<<<< UNREACHABLE >>>>>\n");
}

// ***** Other Operations ***** //

void InterpreterObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  printf("<<<<< ICMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred, inx);

  int v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    Variable *loc1 = executionStack.top()[op1->inx];
    v1 = getValueFromVariable(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    Variable *loc2 = executionStack.top()[op2->inx];
    v2 = getValueFromVariable(loc2);
  }

  int result = 0;
  switch(pred) {
    case CmpInst::ICMP_EQ:
      result = v1 == v2;
      break;
    case CmpInst::ICMP_NE:
      result = v1 != v2;
      break;
    case CmpInst::ICMP_UGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_UGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_ULT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_ULE:
      result = v1 <= v2;
      break;
    case CmpInst::ICMP_SGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_SGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_SLT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_SLE:
      result = v1 <= v2;
      break;
    default:
      break;
  }

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(INT1_KIND, vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  printf("<<<<< FCMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred, inx);


  long double v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    Variable *loc1 = executionStack.top()[op1->inx];
    v1 = getValueFromVariable(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    Variable *loc2 = executionStack.top()[op2->inx];
    v2 = getValueFromVariable(loc2);
  }

  int result = 0;
  switch(pred) {
    case CmpInst::ICMP_EQ:
      result = v1 == v2;
      break;
    case CmpInst::ICMP_NE:
      result = v1 != v2;
      break;
    case CmpInst::ICMP_UGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_UGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_ULT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_ULE:
      result = v1 <= v2;
      break;
    case CmpInst::ICMP_SGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_SGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_SLT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_SLE:
      result = v1 <= v2;
      break;
    default:
      break;
  }

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Variable *nloc = new Variable(INT1_KIND, vresult, false);
  executionStack.top()[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::phinode() {
  printf("<<<<< PHINODE >>>>>\n");
}

void InterpreterObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {
  printf("<<<<< SELECT >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*cond).c_str(), KVALUE_ToString(*tvalue).c_str(), 
      KVALUE_ToString(*fvalue).c_str(), inx);

  cerr << "[InterpreterObserver::select] => Unimplemented\n";
  abort();
}

void InterpreterObserver::push_stack(KVALUE* value) {
  printf("<<<<< PUSH ARGS TO STACK >>>>>");
  printf(" value %s\n", KVALUE_ToString(*value).c_str());
  myStack.push(value);
}

void InterpreterObserver::push_struct_type(KIND kind) {
  printf("<<<<< PUSH STRUCT TYPE >>>>>: %s\n", KIND_ToString(kind).c_str()); 
  structType.push(kind);
}

void InterpreterObserver::push_getelementptr_inx(KVALUE* int_value) {
  int idx = int_value->value.as_int;
  printf("<<<<< PUSH GETELEMENTPTR INDEX >>>>>: %d\n", idx);
  getElementPtrIndexList.push(idx);
}

void InterpreterObserver::push_array_size(uint64_t size) {
  printf("<<<<< PUSH ARRAY SIZE >>>>>: %ld\n", size);
  arraySize.push(size);
}

void InterpreterObserver::call_nounwind(KVALUE* kvalue) {
  printf("<<<<< CALL NOUNWIND >>>>>\n");
  safe_assert(!callerVarIndex.empty());
  Variable* reg = executionStack.top()[callerVarIndex.top()];
  reg->setValue(kvalue->value);
  callerVarIndex.pop();
}

void InterpreterObserver::create_stack_frame(int size) {
  printf("<<<<< CREATE STACK FRAME OF SIZE %d >>>>>\n", size);
  std::vector<Variable*> frame (size);
  for (int i = 0; i < size; i++) {
    frame[i] = new Variable();
  }
  executionStack.push(frame);
}

void InterpreterObserver::call(IID iid, bool nounwind, KIND type, KVALUE* call_value, int inx) {
  // debugging
  printf("<<<<< CALL >>>>> %s, ", IID_ToString(iid).c_str());
  printf(" call_value %s,", KVALUE_ToString(*call_value).c_str());
  printf(" return type %s,", KIND_ToString(type).c_str());
  printf(" nounwind %d,", (nounwind ? 1 : 0));
  printf(" [INX: %d]\n", inx);
  
  if (nounwind) {
    while (!myStack.empty()) {
      myStack.pop();
    }
    while (!callArgs.empty()) {
      callArgs.pop();
    }
  } else {
    while (!myStack.empty()) {
      KVALUE* value = myStack.top();
      myStack.pop();

      // debugging
      printf(", arg: %s", KVALUE_ToString(*value).c_str()); 

      Variable* argCopy;
      if (value->inx != -1) {
        Variable* arg = executionStack.top()[value->inx];
        argCopy= new Variable(arg->getType(), arg->getValue(), true);
      } else {
        argCopy = new Variable(value->kind, value->value, true);
      }
      callArgs.push(argCopy);
    }
  }

  // debugging
  printf("\n");

  callerVarIndex.push(inx);
  executionStack.top()[inx] = new Variable(type, false);

  cout << executionStack.top()[inx]->toString() << "\n";
}


void InterpreterObserver::call_malloc(IID iid, bool nounwind, KIND type, KVALUE* call_value, int size, int inx) {
  // debugging
  printf("<<<<< CALL MALLOC >>>>> %s, call_value: %s, return type: %s, nounwind: %d, size:%d, [INX: %d]", 
      IID_ToString(iid).c_str(), 
      KVALUE_ToString(*call_value).c_str(), 
      KIND_ToString(type).c_str(), 
      (nounwind ? 1 : 0),
      size,
      inx);

  if (type != STRUCT_KIND) {
    // retrieving original number of bytes
    KVALUE* argValue = myStack.top();
    myStack.pop();
    assert(myStack.size() == 0);

    // calculating number of objects
    int numObjects = argValue->value.as_int*8 / size;
    cout << endl << "Number of objects to allocate: " << numObjects << endl;
    
    int actualSize = sizeof(Variable) * numObjects;
    
    // allocating space
    void *addr = malloc(actualSize);
    
    // creating return value
    VALUE returnValue;
    returnValue.as_ptr = addr;    
    executionStack.top()[inx] = new Variable(PTR_KIND, returnValue, size/8, 0, false);
    
    // creating locations
    std::vector<unsigned> offsets(numObjects);
    unsigned currOffset = 0;
    for(int i = 0; i < numObjects; i++) {
      // recording offset
      offsets[i] = currOffset;

      // creating object
      VALUE iValue;
      Variable *var = new Variable(type, iValue, false);
      ((Variable*)addr)[i] = *var;

      // update offset
      currOffset += (size/8);
    }
    mapOffsets[addr] = offsets;
    cout << executionStack.top()[inx]->toString() << endl;
  }
  else {
    cerr << "[Interpreter::call_malloc] => Unimplemented Structs" << endl;
    abort();
  }
  return;
}

void InterpreterObserver::vaarg() {
  printf("<<<<< VAARG >>>>>\n");

  cerr << "[InterpreterObserver::vaarg] => Unimplemented\n";
  abort();
}

void InterpreterObserver::landingpad() {
  printf("<<<<< LANDINGPAD >>>>>\n");

  cerr << "[InterpreterObserver::landingpad] => Unimplemented\n";
  abort();
}

void InterpreterObserver::printCurrentFrame() {
  printf("Print current frame\n");
}
