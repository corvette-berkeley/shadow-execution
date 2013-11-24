
#include "InterpreterObserver.h"

#include <assert.h>
#include <stack>
#include "stdbool.h"
#include <vector>
#include <llvm/IR/InstrTypes.h>


unsigned InterpreterObserver::findIndex(IValue* values, unsigned offset, unsigned length) {
  int low = 0;
  int high = length - 1;
  if (debug)
    cout << "Offset: " << offset << " Length: " << length << endl;

  while(low < high){
    unsigned mid = (low + high) / 2;
    if (debug)
      cout << "mid: " << mid;

    unsigned firstByte = values[mid].getFirstByte();
    if (debug)
      cout << " FirstByte: " << firstByte << endl;
    if (offset == firstByte) {
      if (debug)
        cout << "Returning mid: " << mid << endl;
      return mid;
    }
    else if ((mid + 1 <= length -1) && (firstByte < offset) && (offset < values[mid+1].getFirstByte())) {
      if (debug)
        cout << "Case 2, returning mid: " << mid << endl;
      return mid;
    }
    else if (offset < firstByte) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  if (debug)
    cout << "Returning high: " << high << " " << values[high].getFirstByte() << endl;
  return high; // high
}

void InterpreterObserver::load(IID iid, KIND type, KVALUE* src, int line, int inx) {
  if (debug) {
    printf("<<<<< LOAD >>>>> %s, kind:%s, %s, line %d, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(),
        KVALUE_ToString(src).c_str(),
        line,
        inx);
  }

  IValue* srcPtrLocation;
  if (src->isGlobal) {
    srcPtrLocation = globalSymbolTable[src->inx];
  }
  else {
    srcPtrLocation = executionStack.top()[src->inx];
  }

  if (debug) {
    cout << "\tsrcPtrLocation: " << srcPtrLocation->toString() << endl;
  }
  // creating new value
  IValue *destLocation = new IValue();

  if (srcPtrLocation->isInitialized()) {
    IValue *srcLocation = NULL;
    unsigned srcOffset = srcPtrLocation->getOffset();
    int internalOffset = 0;

    // retrieving source
    IValue* values = (IValue*)srcPtrLocation->getValue().as_ptr;
    unsigned valueIndex = srcPtrLocation->getIndex();
    unsigned currOffset = values[valueIndex].getFirstByte();
    if (debug) {
      cout << "\tvalueIndex: " << valueIndex << " srcOffset: " << srcOffset << " currOffset: " << currOffset << " srcOffset" << srcOffset << endl;
    }
    srcLocation = &values[valueIndex];

    internalOffset = srcOffset - currOffset;
    if (debug) {
      cout << "\tInternal offset: " << internalOffset << endl;
      cout << "\tsrcLocation: " << srcLocation->toString() << endl;
    }

    if (debug)
      cout << "\tCalling readValue with internal offset: " << internalOffset << " and size: " << KIND_GetSize(type) << endl; 
    VALUE value = srcPtrLocation->readValue(internalOffset, type);
    if (debug)
      cout << "\tVALUE returned: " << (float) value.as_flp << endl;

    srcLocation->copy(destLocation);
    destLocation->setSize(KIND_GetSize(type));
    destLocation->setValue(value);
    destLocation->setType(type);

    // sync load
    //bool sync = false;
    bool sync = syncLoad(destLocation, src, type);

    // sync heap if sync value
    if (sync) {
      srcPtrLocation->writeValue(internalOffset, destLocation->getSize(), destLocation);
    }
  } else {
    destLocation->setSize(KIND_GetSize(type));
    destLocation->setType(type);
    destLocation->setLength(0);

    // sync load
    bool sync = syncLoad(destLocation, src, type);

    // sync heap if sync value
    if (sync) {
      VALUE value;
      value.as_ptr = (void*) destLocation;
      srcPtrLocation->setLength(1);
      srcPtrLocation->setValue(value);
      srcPtrLocation->setSize(KIND_GetSize(destLocation->getType()));
    }
  }

  executionStack.top()[inx] = destLocation;
  if (debug)
    cout << destLocation->toString() << endl;

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

long double InterpreterObserver::getValueFromIValue(IValue* loc) {
  KIND kind = loc->getType();
  if (kind == INT1_KIND || kind == INT8_KIND || kind == INT16_KIND || kind == INT32_KIND || kind == INT64_KIND) {
    return loc->getValue().as_int;
  } else {
    return loc->getValue().as_flp;
  }
}

void InterpreterObserver::binop(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx, BINOP op) {
  if (debug)
    printf("<<<<< %s >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", 
        BINOP_ToString(op).c_str(),
        IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(),
        inx);

  long double v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    IValue *loc1 = executionStack.top()[op1->inx];
    v1 = getValueFromIValue(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    IValue *loc2 = executionStack.top()[op2->inx];
    v2 = getValueFromIValue(loc2);
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

  IValue *nloc = new IValue(op1->kind, vresult);
  executionStack.top()[inx] = nloc;
  if (debug)
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
  if (debug)
    printf("<<<<< FREM >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);

  cerr << "[InterpreterObserver::frem] => Unsupported in C???\n";
  abort();
}


// ***** Bitwise Binary Operations ***** //
void InterpreterObserver::shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);

  int value1, value2;
  if (op1->inx == -1) {
    value1 = op1->value.as_int;
  }
  else {
    IValue *loc1 = executionStack.top()[op1->inx];
    value1 = loc1->getValue().as_int;
  }

  if (op2->inx == -1) {
    value2 = op2->value.as_int;
  }
  else {
    IValue *loc2 = executionStack.top()[op2->inx];
    value2 = loc2->getValue().as_int;
  }

  int result = value1 << value2;

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << nloc->toString() << endl;
  }

  return;
}

void InterpreterObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);


  int value1, value2;
  if (op1->inx == -1) {
    value1 = op1->value.as_int;
  }
  else {
    IValue *loc1 = executionStack.top()[op1->inx];
    value1 = loc1->getValue().as_int;
  }

  if (op2->inx == -1) {
    value2 = op2->value.as_int;
  }
  else {
    IValue *loc2 = executionStack.top()[op2->inx];
    value2 = loc2->getValue().as_int;
  }

  int result = value1 >> value2;

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  executionStack.top()[inx] = nloc;
  if (debug) {
    cout << nloc->toString() << endl;
  }

  return;
}

void InterpreterObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);


  int value1, value2;
  if (op1->inx == -1) {
    value1 = op1->value.as_int;
  }
  else {
    IValue *loc1 = executionStack.top()[op1->inx];
    value1 = loc1->getValue().as_int;
  }

  if (op2->inx == -1) {
    value2 = op2->value.as_int;
  }
  else {
    IValue *loc2 = executionStack.top()[op2->inx];
    value2 = loc2->getValue().as_int;
  }

  int result = value1 >> value2;

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  nloc->setSize(KIND_GetSize(op1->kind));
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << nloc->toString() << endl;
  }
  return;
}

void InterpreterObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);

  int value1, value2;
  if (op1->inx == -1) {
    value1 = op1->value.as_int;
  }
  else {
    IValue *loc1 = executionStack.top()[op1->inx];
    value1 = loc1->getValue().as_int;
  }
  
  if (op2->inx == -1) {
    value2 = op2->value.as_int;
  }
  else {
    IValue *loc2 = executionStack.top()[op2->inx];
    value2 = loc2->getValue().as_int;
  }

  int result = value1 & value2;

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << nloc->toString() << endl;
  }
  return;
}

void InterpreterObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);

  int value1, value2;
  if (op1->inx == -1) {
    value1 = op1->value.as_int;
  }
  else {
    IValue *loc1 = executionStack.top()[op1->inx];
    value1 = loc1->getValue().as_int;
  }

  if (op2->inx == -1) {
    value2 = op2->value.as_int;
  }
  else {
    IValue *loc2 = executionStack.top()[op2->inx];
    value2 = loc2->getValue().as_int;
  }

  int result = value1 | value2;

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << nloc->toString() << endl;
  }
  return;
}

void InterpreterObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);

  int value1, value2;
  if (op1->inx == -1) {
    value1 = op1->value.as_int;
  }
  else {
    IValue *loc1 = executionStack.top()[op1->inx];
    value1 = loc1->getValue().as_int;
  }

  if (op2->inx == -1) {
    value2 = op2->value.as_int;
  }
  else {
    IValue *loc2 = executionStack.top()[op2->inx];
    value2 = loc2->getValue().as_int;
  }

  int result = value1 ^ value2;

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);

  executionStack.top()[inx] = nloc;
  if (debug)
    cout << nloc->toString() << endl;

  return;
}

// ***** Vector Operations ***** //

void InterpreterObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< EXTRACTELEMENT >>>>> %s, vector:%s, index:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);

  cerr << "[InterpreterObserver::extractelement] => Unimplemented\n";
  abort();
}

void InterpreterObserver::insertelement() {
  if (debug)
    printf("<<<<< INSERTELEMENT >>>>>\n");

  cerr << "[InterpreterObserver::insertelement] => Unimplemented\n";
  abort();
}

void InterpreterObserver::shufflevector() {
  if (debug)
    printf("<<<<< SHUFFLEVECTOR >>>>>\n");

  cerr << "[InterpreterObserver::shufflevector] => Unimplemented\n";
  abort();
}


// ***** AGGREGATE OPERATIONS ***** //

void InterpreterObserver::extractvalue(IID iid, int inx, int opinx) {
  if (debug)
    printf("<<<<< EXTRACTVALUE >>>>> %s, agg_inx:%d, [INX: %d]\n",
        IID_ToString(iid).c_str(), opinx, inx);

  int index = getElementPtrIndexList.front();
  getElementPtrIndexList.pop();
  safe_assert(getElementPtrIndexList.empty());

  IValue* aggIValue = (opinx == -1) ? NULL : executionStack.top()[opinx];

  KVALUE* aggKValue = returnStruct.front();
  int count = 0;
  while (!returnStruct.empty()) {
    count++;
    returnStruct.pop();
    if (count == index) {
      aggKValue = returnStruct.front();
    }
  }

  cout << "KVALUE: " << KVALUE_ToString(aggKValue) << endl;

  IValue* evValue = new IValue();
  if (aggIValue != NULL) {
    aggIValue += index;
    aggIValue->copy(evValue);
  } else {
    evValue->setType(aggKValue->kind);
    evValue->setValue(aggKValue->value);
    evValue->setLength(0);
  }

  executionStack.top()[inx] = evValue;
  if (debug)
    cout << evValue->toString() << "\n";

  return;
}

void InterpreterObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug)
    printf("<<<<< INSERTVALUE >>>>> %s, vector:%s, value:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);

  cerr << "[InterpreterObserver::insertvalue] => Unimplemented\n";
  abort();
}

// ***** Memory Access and Addressing Operations ***** //

void InterpreterObserver::allocax(IID iid, KIND type, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< ALLOCA >>>>> %s, kind:%s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(type).c_str(), size, inx);

  IValue* ptrLocation;
  IValue* location;
  if (callArgs.empty()) {
    // alloca for non-argument variables
    location = new IValue(type); // should we count it as LOCAL?
    location->setLength(0);
    VALUE value;
    value.as_ptr = location;
    ptrLocation = new IValue(PTR_KIND, value, LOCAL);
    ptrLocation->setSize(KIND_GetSize(type)); // put in constructor
    executionStack.top()[inx] = ptrLocation;
  } else {
    // alloca for function arguments
    location = callArgs.top();
    VALUE value;
    value.as_ptr = (void*) location;
    ptrLocation = new IValue(PTR_KIND, value, LOCAL);
    ptrLocation->setSize(KIND_GetSize(type)); // put in constructor
    executionStack.top()[inx] = ptrLocation;
    callArgs.pop();
  }
  if (debug) {
    cout << "Location: " << location->toString() << endl;
    cout << ptrLocation->toString() << endl;
  }

  return;
}

void InterpreterObserver::allocax_array(IID iid, KIND type, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< ALLOCA_ARRAY >>>>> %s, elemkind:%s, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(type).c_str(), inx);

  unsigned firstByte = 0;
  unsigned bitOffset = 0;
  unsigned length = 0; 
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
    IValue* locArr = (IValue*) malloc(size*structSize*sizeof(IValue));
    for (uint64_t i = 0; i < size; i++) {
      if (type == STRUCT_KIND) {
        for (uint64_t j = 0; j < structSize; j++) {
          IValue* var = new IValue(structKind[j]);
          length++;
          var->setFirstByte(firstByte + bitOffset/8);
          var->setBitOffset(bitOffset%8);
          var->setLength(0);
          unsigned type = structKind[j];
          firstByte += KIND_GetSize(type);
          bitOffset = (type == INT1_KIND) ? bitOffset + 1 : bitOffset;
          locArr[i*structSize+j] = *var;
        }
      } else {
        IValue* var = new IValue(type);
        length++;
        var->setFirstByte(firstByte + bitOffset/8);
        var->setBitOffset(bitOffset%8);
        var->setLength(0);
        firstByte += KIND_GetSize(type);
        bitOffset = (type == INT1_KIND) ? bitOffset + 1 : bitOffset;
        if (type == INT1_KIND) {
          bitOffset++;
        }
        locArr[i] = *var;
      }
    }

    VALUE locArrPtrVal;
    locArrPtrVal.as_ptr = (void*) locArr; 
    IValue* locArrPtr = new IValue(PTR_KIND, locArrPtrVal, LOCAL);
    locArrPtr->setSize(KIND_GetSize(locArr[0].getType()));
    locArrPtr->setLength(length);
    executionStack.top()[inx] = locArrPtr;
  } else {
    IValue *location = callArgs.top();
    VALUE value;
    value.as_ptr = (void*) location;
    IValue* ptrLocation = new IValue(PTR_KIND, value, LOCAL); 
    ptrLocation->setSize(location->getSize());
    executionStack.top()[inx] = ptrLocation;
    callArgs.pop();
  }

  if (debug)
    cout << executionStack.top()[inx]->toString() << endl;

  return;
}

void InterpreterObserver::allocax_struct(IID iid, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< ALLOCA STRUCT >>>>> %s, size: %ld, [INX: %d]\n", IID_ToString(iid).c_str(), size, inx);

  cout << structType.size() << endl;

  //  if (callArgs.empty()) {
  unsigned firstByte = 0;
  unsigned bitOffset = 0;
  unsigned length = 0;
  IValue* ptrToStructVar = (IValue*) malloc(size*sizeof(IValue));
  for (uint64_t i = 0; i < size; i++) {
    KIND type = structType.front();
    IValue* var = new IValue(type);
    var->setFirstByte(firstByte + bitOffset/8);
    var->setBitOffset(bitOffset%8);
    var->setLength(0);
    firstByte += KIND_GetSize(type);
    bitOffset = (type == INT1_KIND) ? bitOffset + 1 : bitOffset;
    length++;
    ptrToStructVar[i] = *var;
    structType.pop();
  }
  safe_assert(structType.empty());

  VALUE structPtrVal;
  structPtrVal.as_ptr = (void*) ptrToStructVar;
  IValue* structPtrVar = new IValue(PTR_KIND, structPtrVal);
  structPtrVar->setSize(KIND_GetSize(ptrToStructVar[0].getType()));
  structPtrVar->setLength(length);

  executionStack.top()[inx] = structPtrVar;
  //  } else {
  //    IValue *location = callArgs.top();
  //    VALUE value;
  //    value.as_ptr = (void*) location;
  //    IValue* ptrLocation = new IValue(PTR_KIND, value, true); 
  //    ptrLocation->setSize(location->getSize());
  //    executionStack.top()[inx] = ptrLocation;
  //    callArgs.pop();
  //  }

  if (debug)
    cout << executionStack.top()[inx]->toString() << "\n";
}

bool InterpreterObserver::checkStore(IValue *dest, KVALUE *kv) {
  bool result = false;

  switch(kv->kind) {
    case PTR_KIND:
      result = true; // don't compare pointer
      // result = (dest->getValue().as_ptr == kv->value.as_ptr);
      break;
    case INT1_KIND: 
      result = ((bool)dest->getValue().as_int == (bool)kv->value.as_int);
      break;
    case INT8_KIND: 
      result = ((int8_t)dest->getValue().as_int == (int8_t)kv->value.as_int);
      break;
    case INT16_KIND: 
      result = ((int16_t)dest->getValue().as_int == (int16_t)kv->value.as_int);
      break;
    case INT32_KIND: 
      result = ((int32_t)dest->getValue().as_int == (int32_t)kv->value.as_int);
      break;
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

void InterpreterObserver::store(IID iid, KVALUE* dest, KVALUE* src, int line, int inx) {
  if (debug)
    printf("<<<<< STORE >>>>> %s, %s, %s, line: %d, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(dest).c_str(), //op
        KVALUE_ToString(src).c_str(), line, inx); // kv

  // retrieve ptr destination
  IValue* destPtrLocation;
  if (dest->isGlobal) {
    destPtrLocation = globalSymbolTable[dest->inx];
  }
  else {
    destPtrLocation = executionStack.top()[dest->inx];
  }

  if (debug)
    cout << "\tDestPtr: " << destPtrLocation->toString() << endl;

  // TODO: Review this
  if (!destPtrLocation->isInitialized()) {
    IValue* iValue = new IValue(src->kind);
    iValue->setLength(0);
    VALUE value;
    value.as_ptr = (void*) iValue;
    destPtrLocation->setValue(value);
    destPtrLocation->setInitialized();
  }

  unsigned destPtrOffset = destPtrLocation->getOffset();
  IValue *destLocation = NULL;
  IValue *srcLocation = NULL;
  int internalOffset = 0;

  // retrieve source
  srcLocation = (src->iid == 0) ? new IValue(src->kind, src->value) : executionStack.top()[src->inx];

  if (debug)
    cout << "\tSrc: " << srcLocation->toString() << endl;

  // retrieve actual destination
  IValue* values = (IValue*)destPtrLocation->getValue().as_ptr;
  unsigned valueIndex = destPtrLocation->getIndex();
  unsigned currOffset = values[valueIndex].getFirstByte();
  destLocation = &values[valueIndex];
  internalOffset = destPtrOffset - currOffset;

  if (debug) {
  cout << "\tdestPtrOffset: " << destPtrOffset << endl;
  cout << "\tvalueIndex: " << valueIndex << " currOffset: " << currOffset <<  " Other offset: "  << destPtrOffset << endl;
  cout << "\tinternalOffset: " << internalOffset <<  " Size: " << destPtrLocation->getSize() << endl;

  cout << "\tDest: " << destLocation->toString() << endl;

  cout << "\tCalling writeValue with offset: " << internalOffset << ", size: " << destPtrLocation->getSize() << endl;
  }

  // writing src into dest
  // destPtrLocation->writeValue(internalOffset, destPtrLocation->getSize(), srcLocation);

  destPtrLocation->writeValue(internalOffset, KIND_GetSize(src->kind), srcLocation);
  destPtrLocation->setInitialized();


  if (debug)
    cout << "\tUpdated Dest: " << destLocation->toString() << endl;

  // just read again to check store
  if (debug)
    cout << "\tCalling readValue with internal offset: " << internalOffset << " size: " << destPtrLocation->getSize() << endl;
  IValue* writtenValue = new IValue(srcLocation->getType(), destPtrLocation->readValue(internalOffset, src->kind)); // NOTE: destLocation->getType() before
  if (debug)
    cout << "\twrittenValue: " << writtenValue->toString() << endl;
  if (!checkStore(writtenValue, src)) { // destLocation
    cerr << "\tKVALUE: " << KVALUE_ToString(src) << endl;
    cerr << "\tMismatched values found in Store" << endl;
    abort();
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
      KVALUE_ToString(kv1).c_str(),
      KVALUE_ToString(kv2).c_str(), 
      inx);

  cerr << "[InterpreterObserver::cmpxchg] => Unimplemented\n";
  abort();
}

void InterpreterObserver::atomicrmw() {
  printf("<<<<< ATOMICRMW >>>>>\n");

  cerr << "[InterpreterObserver::atomicrmw] => Unimplemented\n";
  abort();
}

void InterpreterObserver::getelementptr(IID iid, bool inbound, KVALUE* base, KVALUE* offset, KIND type, uint64_t size, int line, int inx) {
  if (debug)
    printf("<<<<< GETELEMENTPTR= >>>>> %s, inbound:%s, pointer_value:%s, index:%s, kind: %s, size: %ld, line: %d, [INX: %d]\n", 
        IID_ToString(iid).c_str(),
        (inbound ? "1" : "0"),
        KVALUE_ToString(base).c_str(),
        KVALUE_ToString(offset).c_str(),
        KIND_ToString(type).c_str(),
        size,
        line,
        inx);

  IValue* basePtrLocation = executionStack.top()[base->inx];
  IValue* ptrLocation;

  if (basePtrLocation->isInitialized()) {
    int offsetValue;
    if (offset->inx != -1) {
      offsetValue = executionStack.top()[offset->inx]->getValue().as_int;
    }
    else {
      offsetValue = offset->value.as_int;
    }

    // offset in bytes from base ptr
    unsigned newOffset = (offsetValue * (size/8)) + basePtrLocation->getOffset();
    cout << "newOffset: " << newOffset << endl;

    unsigned index = findIndex((IValue*) basePtrLocation->getValue().as_ptr, newOffset, basePtrLocation->getLength()); // TODO: revise offset
    ptrLocation = new IValue(PTR_KIND, basePtrLocation->getValue(), size/8, newOffset, index, basePtrLocation->getLength());
  } else {
    ptrLocation = new IValue(PTR_KIND, basePtrLocation->getValue(), size/8, 0, 0, 0);
  }
  
  executionStack.top()[inx] = ptrLocation;
  if (debug)
    cout << executionStack.top()[inx]->toString() << endl;

  return;
}

void InterpreterObserver::getelementptr_array(IID iid, bool inbound, KVALUE* op, KIND kind, int inx) {
  if (debug)
    printf("<<<<< GETELEMENTPTR_ARRAY >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, [INX: %d]\n", 
        IID_ToString(iid).c_str(),
        (inbound ? "1" : "0"),
        KVALUE_ToString(op).c_str(),
        KIND_ToString(kind).c_str(),
        inx);

  IValue* ptrArray = executionStack.top()[op->inx];
  IValue* array = static_cast<IValue*>(ptrArray->getValue().as_ptr);

  getElementPtrIndexList.pop();

  int index = 1;
  arraySize.pop();
  while (!arraySize.empty()) {
    index = index * arraySize.front();
    arraySize.pop();
  }
  index = getElementPtrIndexList.front()*index;
  getElementPtrIndexList.pop();
  safe_assert(getElementPtrIndexList.empty());

  index = ptrArray->getIndex() + index;

  cout << "Getting element at index : " << index << endl;

  IValue* arrayElem = array + index;
  IValue* arrayElemPtr = new IValue(PTR_KIND, ptrArray->getValue());
  arrayElemPtr->setIndex(index);
  arrayElemPtr->setLength(ptrArray->getLength());
  arrayElemPtr->setSize(KIND_GetSize(arrayElem[0].getType()));
  arrayElemPtr->setOffset(arrayElem[0].getFirstByte());

  executionStack.top()[inx] = arrayElemPtr;
  if (debug)
    cout << executionStack.top()[inx]->toString() << endl;
}

void InterpreterObserver::getelementptr_struct(IID iid, bool inbound, KVALUE* op, KIND kind, KIND arrayKind, int inx) {
  if (debug)
    printf("<<<<< GETELEMENTPTR_STRUCT >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, arrayKind: %s, [INX: %d]\n", 
        IID_ToString(iid).c_str(),
        (inbound ? "1" : "0"),
        KVALUE_ToString(op).c_str(),
        KIND_ToString(kind).c_str(),
        KIND_ToString(arrayKind).c_str(),
        inx);

  IValue* structPtr = executionStack.top()[op->inx];
  IValue* structElemPtr;

  cout << structPtr->toString() << endl;

  if (structPtr->isInitialized()) {
    IValue* structBase = static_cast<IValue*>(structPtr->getValue().as_ptr);

    int index;
    getElementPtrIndexList.pop();
    index = getElementPtrIndexList.front();
    getElementPtrIndexList.pop();
    index = structPtr->getIndex() + index;

    cout << "Getting element at index: " << index << endl;

    // TODO: revisit this
    if (index < (int) structPtr->getLength()) {
      IValue* structElem = structBase + index;
      structElemPtr = new IValue(PTR_KIND, structPtr->getValue());
      structElemPtr->setIndex(index);
      structElemPtr->setLength(structPtr->getLength());
      structElemPtr->setSize(KIND_GetSize(structElem->getType()));
      structElemPtr->setOffset(structElem->getFirstByte());
    } else {
      structElemPtr = new IValue(PTR_KIND, structPtr->getValue(), structPtr->getSize(), 0, 0, 0);
    }
  } else {
    structElemPtr = new IValue(PTR_KIND, structPtr->getValue(), structPtr->getSize(), 0, 0, 0);
  }

  executionStack.top()[inx] = structElemPtr;

  if (debug)
    cout << executionStack.top()[inx]->toString() << "\n";
}

// ***** Conversion Operations ***** //

void InterpreterObserver::trunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< TRUNC >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);

  IValue* src = executionStack.top()[op->inx];
  VALUE value = src->getValue();
  int64_t intValue = value.as_int;
  int64_t* int64Ptr = &intValue;
  bool* int1Ptr = (bool*) int64Ptr;
  int8_t* int8Ptr = (int8_t*) int64Ptr;
  int16_t* int16Ptr = (int16_t*) int64Ptr;
  int32_t* int32Ptr = (int32_t*) int64Ptr;
  VALUE truncValue;

  switch (type) {
    case INT1_KIND:
      truncValue.as_int = *int1Ptr;
      break;
    case INT8_KIND:
      truncValue.as_int = *int8Ptr; 
      break;
    case INT16_KIND:
      truncValue.as_int = *int16Ptr;
      break;
    case INT32_KIND:
      truncValue.as_int = *int32Ptr;
      break;
    case INT64_KIND:
      truncValue.as_int = *int64Ptr;
      break;
    default:
      cerr << "[InterpreterObserver::zext] => Unsupport integer type " << type << "\n";
      safe_assert(false);
  }

  IValue* truncVar = new IValue(type, truncValue);
  executionStack.top()[inx] = truncVar;
  if (debug)
    cout << executionStack.top()[inx]->toString() << endl;
}

void InterpreterObserver::zext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< ZEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);

  IValue *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();
  int64_t intValue = value.as_int;
  if (op->kind == INT1_KIND) {
    intValue = (bool) intValue;
  }
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

  IValue* zextVar = new IValue(type, zextValue);
  executionStack.top()[inx] = zextVar;
  if (debug)
    cout << executionStack.top()[inx]->toString() << "\n";
}

void InterpreterObserver::sext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< SEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);

  /*
     cerr << "[InterpreterObserver::sext] => Unimplemented.\n";
     abort();
     */
  IValue *src = executionStack.top()[op->inx];
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

  IValue *ext_loc = new IValue(type, ext_value);
  executionStack.top()[inx] = ext_loc;
  if (debug)
    cout << ext_loc->toString() << "\n";
}

void InterpreterObserver::fptrunc(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< FPTRUNC >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);

  IValue *src = executionStack.top()[kv->inx];
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

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug)
    cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fpext(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< FPEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);

  IValue *src = executionStack.top()[kv->inx];
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

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug)
    cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fptoui(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< FPTOUII >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);

  IValue *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug)
    cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fptosi(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< FPTOSI >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);

  IValue *src = executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug)
    cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::uitofp(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< UITOFP >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);

  IValue *src = executionStack.top()[kv->inx];
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

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug)
    cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::sitofp(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< SITOFP >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);

  IValue *src = executionStack.top()[kv->inx];
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

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug)
    cout << trunc_loc->toString() << "\n";

}

void InterpreterObserver::ptrtoint(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< PTRTOINT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);

  IValue *src = executionStack.top()[op->inx];
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

  IValue *ptrToInt = new IValue(type, int_value);
  executionStack.top()[inx] = ptrToInt;
  if (debug)
  cout << executionStack.top()[inx]->toString() << "\n";
}

void InterpreterObserver::inttoptr(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< INTTOPTR >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);

  IValue *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();
  VALUE ptr_value;
  ptr_value.as_ptr = value.as_ptr;
  IValue *intToPtr = new IValue(type, ptr_value);
  executionStack.top()[inx] = intToPtr;
  cout << executionStack.top()[inx]->toString() << "\n";
}

void InterpreterObserver::bitcast(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug)
    printf("<<<<< BITCAST >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);

  IValue *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();

  IValue *bitcastLoc = new IValue(type, value, size/8, src->getOffset(), src->getIndex(), src->getLength()); // TODO: check
  executionStack.top()[inx] = bitcastLoc;
  if (debug)
    cout << bitcastLoc->toString() << endl;
  return;
}


// ***** TerminatorInst ***** //
void InterpreterObserver::branch(IID iid, bool conditional, KVALUE* op1, int inx) {
  if (debug)
    printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (conditional ? "1" : "0"),
        KVALUE_ToString(op1).c_str(), inx);
}

void InterpreterObserver::branch2(IID iid, bool conditional, int inx) {
  if (debug)
    printf("<<<<< BRANCH >>>>> %s, cond: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (conditional ? "1" : "0"), inx);
}

void InterpreterObserver::indirectbr(IID iid, KVALUE* op1, int inx) {
  if (debug)
    printf("<<<<< INDIRECTBR >>>>> %s, address: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(), inx);
}

void InterpreterObserver::invoke(IID iid, KVALUE* call_value, int inx) {
  if (debug)
    printf("<<<<< INVOKE >>>>> %s, call_value: %s, [INX: %d]", IID_ToString(iid).c_str(), KVALUE_ToString(call_value).c_str(), inx);
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    if (debug)
      printf(", arg: %s", KVALUE_ToString(value).c_str()); 
  }
  if (debug)
    printf("\n");

  cerr << "[InterpreterObserver::invoke] => Unimplemented\n";
  abort();
}

void InterpreterObserver::resume(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< RESUME >>>>> %s, acc_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(op1).c_str(), inx);

  cerr << "[InterpreterObserver::resume] => Unimplemented\n";
  abort();
}

void InterpreterObserver::return_(IID iid, KVALUE* op1, int inx) {
  if (debug)
    printf("<<<<< RETURN 1>>>>> %s, ret_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(), inx);

  safe_assert(!executionStack.empty());

  IValue* returnValue = op1->inx == -1 ? NULL : executionStack.top()[op1->inx];

  executionStack.pop();

  if (!executionStack.empty()) {
    if (debug)
      cout << "New stack size: " << executionStack.size() << "\n";
    safe_assert(!callerVarIndex.empty());

    if (returnValue == NULL) {
      executionStack.top()[callerVarIndex.top()]->setValue(op1->value); 
      executionStack.top()[callerVarIndex.top()]->setType(op1->kind); 
    } else {
      executionStack.top()[callerVarIndex.top()]->setValue(returnValue->getValue()); 
      executionStack.top()[callerVarIndex.top()]->setType(returnValue->getType()); 
    }
    if (debug)
      cout << executionStack.top()[callerVarIndex.top()]->toString() << "\n";

    callerVarIndex.pop();
  } else {
    cout << "The execution stack is empty.\n";
  }

  isReturn = true;

  return;
}

void InterpreterObserver::return2_(IID iid, int inx) {
  if (debug)
    printf("<<<<< RETURN 2>>>>> %s, [INX: %d]\n", IID_ToString(iid).c_str(), inx);

  safe_assert(!executionStack.empty());
  executionStack.pop();

  if (!executionStack.empty()) {
    if (debug)
      cout << "New stack size: " << executionStack.size() << "\n";
  } else {
    cout << "The execution stack is empty.\n";
  }

  isReturn = true;

  return;
}

void InterpreterObserver::return_struct_(IID iid, int inx, int valInx) {
  if (debug)
    printf("<<<<< RETURN STRUCT >>>>> %s, val_inx: %d, [INX: %d]\n", IID_ToString(iid).c_str(), valInx, inx);

  safe_assert(!executionStack.empty());

  IValue* returnValue = (valInx == -1) ? NULL : executionStack.top()[valInx];

  executionStack.pop();

  if (!executionStack.empty()) {
    cout << "New stack size: " << executionStack.size() << "\n";
    safe_assert(!callerVarIndex.empty());
    safe_assert(!returnStruct.empty());

    // reconstruct struct value
    IValue* structValue = (IValue*) malloc(returnStruct.size()*sizeof(IValue));
    unsigned i = 0;
    while (!returnStruct.empty()) {
      KVALUE* value = returnStruct.front();
      IValue* iValue;

      if (returnValue == NULL) {
        iValue = new IValue(value->kind);
        iValue->setValue(value->value);
        iValue->setLength(0);
      } else {
        iValue = new IValue();
        returnValue->copy(iValue);
        returnValue++;
      }

      structValue[i] = *iValue; 
      i++;
      returnStruct.pop();
    }

    safe_assert(returnStruct.empty());

    executionStack.top()[callerVarIndex.top()] = structValue;
    if (debug)
      cout << executionStack.top()[callerVarIndex.top()]->toString() << "\n";
  } else {
    cout << "The execution stack is empty.\n";
  }

  safe_assert(!callerVarIndex.empty());
  callerVarIndex.pop();

  isReturn = true;

  return;
}

void InterpreterObserver::switch_(IID iid, KVALUE* op, int inx) {
  if (debug)
    printf("<<<<< SWITCH >>>>> %s, condition: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op).c_str(), inx);
  abort();
}

void InterpreterObserver::unreachable() {
  printf("<<<<< UNREACHABLE >>>>>\n");
}

// ***** Other Operations ***** //

void InterpreterObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  if (debug)
    printf("<<<<< ICMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(op1).c_str(), KVALUE_ToString(op2).c_str(), pred, inx);

  int v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    IValue *loc1 = executionStack.top()[op1->inx];
    v1 = getValueFromIValue(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    IValue *loc2 = executionStack.top()[op2->inx];
    v2 = getValueFromIValue(loc2);
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

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  nloc->setSize(KIND_GetSize(op1->kind));

  executionStack.top()[inx] = nloc;
  if (debug) {
    cout << nloc->toString() << endl;
  }
  return;
}

void InterpreterObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  if (debug)
    printf("<<<<< FCMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(op1).c_str(), KVALUE_ToString(op2).c_str(), pred, inx);


  long double v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    IValue *loc1 = executionStack.top()[op1->inx];
    v1 = getValueFromIValue(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    IValue *loc2 = executionStack.top()[op2->inx];
    v2 = getValueFromIValue(loc2);
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

  IValue *nloc = new IValue(INT1_KIND, vresult);
  executionStack.top()[inx] = nloc;
  if (debug)
    cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::phinode(IID iid, int inx) {
  if (debug)
    printf("<<<<< PHINODE >>>>>: id %s [INX: %d] \n", IID_ToString(iid).c_str(), inx);

  cout << recentBlock.top() << endl;

  IValue* phiNode;

  if (phinodeConstantValues.find(recentBlock.top()) != phinodeConstantValues.end()) {
    KVALUE* constant = phinodeConstantValues[recentBlock.top()];
    phiNode = new IValue(constant->kind, constant->value);
    phiNode->setLength(0);
  } else {
    safe_assert(phinodeValues.find(recentBlock.top()) != phinodeValues.end());
    IValue* inValue = executionStack.top()[phinodeValues[recentBlock.top()]];
    phiNode = new IValue();
    inValue->copy(phiNode);
  }

  phinodeConstantValues.clear();
  phinodeValues.clear();

  executionStack.top()[inx] = phiNode;
  if (debug)
    cout << phiNode->toString() << "\n";

  return;
}

void InterpreterObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {

  cerr << "[InterpreterObserver::select] => Unimplemented\n";
  safe_assert(false);

  if (debug) {
    printf("<<<<< SELECT >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(cond).c_str(), KVALUE_ToString(tvalue).c_str(), 
        KVALUE_ToString(fvalue).c_str(), inx);
  }
}

void InterpreterObserver::push_stack(KVALUE* value) {
  if (debug)
    printf("<<<<< PUSH ARGS TO STACK >>>>>, value %s\n", KVALUE_ToString(value).c_str());
  myStack.push(value);
}

void InterpreterObserver::push_phinode_constant_value(KVALUE* value, int blockId) {
  if (debug)
    printf("<<<<< PUSH PHINODE CONSTANT VALUE >>>>> kvalue: %s, blockid: %d\n", KVALUE_ToString(value).c_str(), blockId);
  phinodeConstantValues[blockId] = value;
}

void InterpreterObserver::push_phinode_value(int valId, int blockId) {
  if (debug)
    printf("<<<<< PUSH PHINODE VALUE >>>>> valId: %d, blockid: %d\n", valId, blockId);
  phinodeValues[blockId] = valId;
}

void InterpreterObserver::push_return_struct(KVALUE* value) {
  if (debug)
    printf("<<<<< PUSH RETURN STRUCT >>>>> value %s\n", KVALUE_ToString(value).c_str());
  returnStruct.push(value);
}

void InterpreterObserver::push_struct_type(KIND kind) {
  if (debug)
    printf("<<<<< PUSH STRUCT TYPE >>>>>: %s\n", KIND_ToString(kind).c_str()); 
  structType.push(kind);
  // TODO: this seems to be a bug in C++ queue
  // that requires us to do this
  if (structType.size() == 0) {
    structType.push(kind);
  }
}

void InterpreterObserver::push_getelementptr_inx(KVALUE* int_value) {
  int idx = int_value->value.as_int;
  if (debug)
    printf("<<<<< PUSH GETELEMENTPTR INDEX >>>>>: %d\n", idx);
  getElementPtrIndexList.push(idx);
}

void InterpreterObserver::push_getelementptr_inx2(int int_value) {
  int idx = int_value;
  if (debug)
    printf("<<<<< PUSH GETELEMENTPTR INDEX >>>>>: %d\n", idx);
  getElementPtrIndexList.push(idx);
}

void InterpreterObserver::push_array_size(uint64_t size) {
  if (debug)
    printf("<<<<< PUSH ARRAY SIZE >>>>>: %ld\n", size);
  arraySize.push(size);
}

void InterpreterObserver::after_call(KVALUE* kvalue) {
  if (debug) {
    printf("<<<<< AFTER CALL >>>>> kvalue: %s\n", KVALUE_ToString(kvalue).c_str());
  }

  if (!isReturn) {
    // call is not interpreted
    safe_assert(!callerVarIndex.empty());

    // empty myStack and callArgs
    while (!myStack.empty()) {
      myStack.pop();
    }
    while (!callArgs.empty()) {
      callArgs.pop();
    }

    IValue* reg = executionStack.top()[callerVarIndex.top()];
    reg->setValue(kvalue->value);
    callerVarIndex.pop();

    if (debug) {
      cout << reg->toString() << endl;
    }
  } else {
    safe_assert(myStack.empty());
    safe_assert(callArgs.empty());
  }

  isReturn = false;

  safe_assert(!recentBlock.empty());
  recentBlock.pop();
}

void InterpreterObserver::after_void_call() {
  if (debug) {
    printf("<<<<< AFTER VOID CALL >>>>>");
  }

  isReturn = false;
}

void InterpreterObserver::create_stack_frame(int size) {
  if (debug) {
    printf("<<<<< CREATE STACK FRAME OF SIZE %d >>>>>\n", size);
  }
  std::vector<IValue*> frame (size);
  for (int i = 0; i < size; i++) {
    frame[i] = new IValue();
  }
  executionStack.push(frame);
}

void InterpreterObserver::create_global_symbol_table(int size) {
  if (debug)
    printf("<<<<< CREATE GLOBAL SYMBOL TABLE OF SIZE %d >>>>>\n", size);

  for (int i = 0; i < size; i++) {
    IValue* value = new IValue();
    globalSymbolTable.push_back(value);
  }
}

void InterpreterObserver::record_block_id(int id) {
  if (debug)
    printf("<<<<< RECORD BLOCK ID >>>>> %d\n", id);
  if (recentBlock.empty()) {
    recentBlock.push(id);
  } else {
    recentBlock.pop();
    recentBlock.push(id);
  }
}

void InterpreterObserver::create_global(KVALUE* kvalue, KVALUE* initializer) {
  if (debug) {
    printf("<<<<< CREATE GLOBAL >>>>> %s %s\n", KVALUE_ToString(kvalue).c_str(), KVALUE_ToString(initializer).c_str());
  }

  // allocate object
  IValue* location;
  location = new IValue(initializer->kind, initializer->value, GLOBAL); // GLOBAL?
  location->setLength(0);

  VALUE value;
  value.as_ptr = location;
  IValue* ptrLocation = new IValue(PTR_KIND, value, GLOBAL);
  ptrLocation->setSize(KIND_GetSize(initializer->kind)); // put in constructor

  // store it in globalSymbolTable
  globalSymbolTable[kvalue->inx] = ptrLocation;
  cout << "\tloc: " << location->toString() << endl;
  cout << "\tptr: " << ptrLocation->toString() << endl;
}

void InterpreterObserver::call(IID iid, bool nounwind, KIND type, int inx) {
  if (debug) {
    printf("<<<<< CALL >>>>> %s, ", IID_ToString(iid).c_str());
    printf(" return type %s,", KIND_ToString(type).c_str());
    printf(" nounwind %d,", (nounwind ? 1 : 0));
    printf(" [INX: %d]\n", inx);
  }

  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();

    // debugging
    if (debug) {
      cout << ", arg: " << KVALUE_ToString(value).c_str();
    }

    IValue* argCopy;
    if (value->inx != -1) {
      IValue* arg = (IValue*)executionStack.top()[value->inx];
      safe_assert(arg);
      argCopy = new IValue();
      arg->copy(argCopy);
    } else {
      argCopy = new IValue(value->kind, value->value, LOCAL);
    }
    callArgs.push(argCopy);
  }

  // debugging
  if (debug) {
    cout << endl;
  }

  if (type != VOID_KIND) {
    callerVarIndex.push(inx); 
  }

  IValue* callValue = new IValue(type);
  callValue->setLength(0);
  executionStack.top()[inx] = callValue;

  if (debug) {
    cout << executionStack.top()[inx]->toString() << "\n";
  }

  // new recentBLock stack frame for the new call
  recentBlock.push(0);
}


void InterpreterObserver::call_malloc(IID iid, bool nounwind, KIND type, KVALUE* call_value, int size, int inx) {

  if (debug)
    printf("<<<<< CALL MALLOC >>>>> %s, call_value: %s, return type: %s, nounwind: %d, size:%d, [INX: %d]", 
        IID_ToString(iid).c_str(), 
        KVALUE_ToString(call_value).c_str(), 
        KIND_ToString(type).c_str(), 
        (nounwind ? 1 : 0),
        size,
        inx);

  if (type != STRUCT_KIND) {
    // retrieving original number of bytes
    KVALUE* argValue = myStack.top();
    myStack.pop();
    assert(myStack.size() == 0);

    // allocating space
    int numObjects = argValue->value.as_int*8 / size;
    int actualSize = sizeof(IValue) * numObjects;    
    void *addr = malloc(actualSize);

    // creating pointer object
    VALUE returnValue;
    returnValue.as_ptr = addr;    
    executionStack.top()[inx] = new IValue(PTR_KIND, returnValue, size/8, 0, 0, numObjects);

    // creating locations
    unsigned currOffset = 0;
    for(int i = 0; i < numObjects; i++) {
      // creating object
      VALUE iValue;
      IValue *var = new IValue(type, iValue, currOffset);
      ((IValue*)addr)[i] = *var;

      // updating offset
      currOffset += (size/8);
    }
    if (debug)
      cout << endl << executionStack.top()[inx]->toString() << endl;
  }
  else {

    unsigned size = structType.size();
    cout << "\nNumber of fields: " << size << endl;
    unsigned firstByte = 0;
    unsigned length = 0;
    IValue* ptrToStructVar = (IValue*) malloc(size*sizeof(IValue));
    for (unsigned i = 0; i < size; i++) {
      KIND type = structType.front();
      IValue* var = new IValue(type);
      var->setFirstByte(firstByte);
      firstByte += KIND_GetSize(type);
      length++;
      ptrToStructVar[i] = *var;
      structType.pop();
    }
    safe_assert(structType.empty());

    VALUE structPtrVal;
    structPtrVal.as_ptr = (void*) ptrToStructVar;
    IValue* structPtrVar = new IValue(PTR_KIND, structPtrVal);
    structPtrVar->setSize(KIND_GetSize(ptrToStructVar[0].getType()));
    structPtrVar->setLength(length);

    executionStack.top()[inx] = structPtrVar;
    if (debug)
      cout << structPtrVar->toString() << endl;
    //cerr << "[Interpreter::call_malloc=] => Unimplemented Structs" << endl;
    //abort();
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

bool InterpreterObserver::syncLoad(IValue* iValue, KVALUE* concrete, KIND type) { 
  bool sync = false;
  VALUE syncValue;
  long cValueInt;
  float cValueFloat;
  double cValueDouble;
  long double cValueLD;

  switch (type) {
    case PTR_KIND:
      break;
    case INT1_KIND: 
      // TODO: we don't actually have this case, do we?
      safe_assert(false);
    case INT8_KIND: 
      cValueInt = *((int8_t*) concrete->value.as_ptr);
      sync = (iValue->getValue().as_int != cValueInt);
      if (sync) {
        syncValue.as_int = cValueInt;
        iValue->setValue(syncValue);
      }
      break;
    case INT16_KIND: 
      cValueInt = *((int16_t*) concrete->value.as_ptr);
      sync = (iValue->getValue().as_int != cValueInt);
      if (sync) {
        syncValue.as_int = cValueInt;
        iValue->setValue(syncValue);
      }
      break;
    case INT32_KIND: 
      cValueInt = *((int32_t*) concrete->value.as_ptr);
      sync = (iValue->getValue().as_int != cValueInt);
      if (sync) {
        if (debug) {
          cout << "\t IVALUE: " << iValue->getValue().as_int << endl; 
          cout << "\t CONCRETE: " << cValueInt << endl; 
        }
        syncValue.as_int = cValueInt;
        iValue->setValue(syncValue);
      }
      break;
    case INT64_KIND:
      cValueInt = *((int64_t*) concrete->value.as_ptr);
      sync = (iValue->getValue().as_int != cValueInt);
      if (sync) {
        syncValue.as_int = cValueInt;
        iValue->setValue(syncValue);
      }
      break;
    case FLP32_KIND:
      cValueFloat = *((float*) concrete->value.as_ptr);
      sync = ((float)iValue->getValue().as_flp != cValueFloat);
      if (sync) {
        syncValue.as_flp = cValueFloat;
        iValue->setValue(syncValue);
      }
      break;
    case FLP64_KIND:
      cValueDouble = *((double*) concrete->value.as_ptr);
      sync = ((double)iValue->getValue().as_flp != cValueDouble);
      if (sync) {
        syncValue.as_flp = cValueDouble;
        iValue->setValue(syncValue);
      }
      break;
    case FLP80X86_KIND:
      cValueLD = *((long double*) concrete->value.as_ptr);
      sync = ((long double)iValue->getValue().as_flp != cValueLD);
      if (sync) {
        syncValue.as_flp = cValueLD;
        iValue->setValue(syncValue);
      }
      break;
    default: 
      cout << "Should not reach here!" << endl;
      safe_assert(false);
      break;
  }

  if (sync) {
    if (debug) {
      cout << "\t SYNCING AT LOAD DUE TO MISMATCH" << endl;
      cout << "\t " << iValue->toString() << endl;
    }
  }

  return sync;
}
