
#include "InterpreterObserver.h"

#include <assert.h>
#include <glog/logging.h>
#include <math.h>
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
  if (debug) {
    cout << "\t\tReturning high: " << high << " " << "values[high] = " << values[high].toString() << endl;
    cout << "\t\tReturning high: " << high << " " << values[high].getFirstByte() << endl;
  }
  return high; // high
}

void InterpreterObserver::load_struct(IID iid, KIND type, KVALUE* src, int line, int inx) {
  if (debug) {
    printf("<<<<< LOAD STRUCT >>>>> %s, kind:%s, %s, line %d, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(),
        KVALUE_ToString(src).c_str(),
        line,
        inx);
  }

  int i, structSize;
  IValue* dest;

  structSize = returnStruct.size();
  dest = (IValue*) malloc(structSize*sizeof(IValue));

  if (src->inx == -1) {

    //
    // Case 1: struct constant.
    //
    // Create an IValue struct that has all values in structReturn.
    //
    
    i = 0;
    while (!returnStruct.empty()) {
      KVALUE* concreteStructElem; 
      IValue* structElem; 
      
      concreteStructElem = returnStruct.front();

      if (concreteStructElem->inx == -1) {
        structElem = new IValue(concreteStructElem->kind, concreteStructElem->value,
            REGISTER);
      } else {
        structElem = concreteStructElem->isGlobal ?
          globalSymbolTable[concreteStructElem->inx] :
          executionStack.top()[concreteStructElem->inx];
      }

      dest[i] = *structElem;

      i++;
      returnStruct.pop();
    }
    
    safe_assert(false);

  } else {
    
    //
    // Case 2: local or global struct.
    //
    
    IValue* srcPointer;
    IValue* structSrc;

    srcPointer = src->isGlobal ? globalSymbolTable[src->inx] : executionStack.top()[src->inx];
    structSrc = (IValue*) srcPointer->getIPtrValue();

    i = 0;
    while (!returnStruct.empty()) {
      KVALUE *concreteStructElem, *concreteStructElemPtr; 
      IValue *structElem;
      int type;

      //
      // get concrete value in case we need to sync
      //
      concreteStructElem = returnStruct.front();

      structElem = new IValue();
      structSrc[i].copy(structElem);
      type = structElem->getType();

      //
      // sync load
      // first create a KVALUE pointer to concreteStructElem because sync load
      // expect the KVALUE to be a pointer to the concrete value
      //
      concreteStructElemPtr = (KVALUE*) malloc(sizeof(KVALUE));
      concreteStructElemPtr->value.as_ptr = &(concreteStructElem->value);
      syncLoad(structElem, concreteStructElemPtr, type);

      dest[i] = *structElem;

      i++;
      returnStruct.pop();
    }
  }

  dest->setLineNumber(line);

  executionStack.top()[inx] = dest;

  if (debug) cout << "Destination result: " << dest->toString() << endl;

  return;
}

void InterpreterObserver::load(IID iid, KIND type, KVALUE* src, int line, int inx) {
  if (debug) {
    printf("<<<<< LOAD >>>>> %s, kind:%s, %s, line %d, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(),
        KVALUE_ToString(src).c_str(),
        line,
        inx);
  }

  bool isPointerConstant = false;
  IValue* srcPtrLocation;

  // obtain source pointer value
  if (src->inx == -1) {
    // cannot load using constant address
    isPointerConstant = true;
  } else if (src->isGlobal) {
    srcPtrLocation = globalSymbolTable[src->inx];
  } else {
    srcPtrLocation = executionStack.top()[src->inx];
  }

  // perform load
  if (!isPointerConstant) {

    if (debug) {
      cout << "\tsrcPtrLocation: " << srcPtrLocation->toString() << endl;
    }

    // creating new value
    IValue *destLocation = new IValue();    
    if (srcPtrLocation->isInitialized()) {
      IValue *srcLocation = NULL;

      // retrieving source
      IValue* values = (IValue*)srcPtrLocation->getIPtrValue();
      unsigned valueIndex = srcPtrLocation->getIndex();
      unsigned currOffset = values[valueIndex].getFirstByte();
      srcLocation = values + valueIndex;

      // calculating internal offset
      unsigned srcOffset = srcPtrLocation->getOffset();
      int internalOffset = srcOffset - currOffset;
      VALUE value = srcPtrLocation->readValue(internalOffset, type);

      if (debug) {
        cout << "\tIndex values:" << endl;
        cout << "\t\tvalueIndex: " << valueIndex << endl;
        cout << "\t\tsrcOffset: " << srcOffset << endl;
        cout << "\t\tcurrOffset: " << currOffset << endl;
        cout << "\t\tsrcOffset" << srcOffset << endl;
        cout << "\t\tinternal offset: " << internalOffset << endl;
        cout << "\tsrcLocation: " << srcLocation->toString() << endl;
        cout << "\tCalling readValue with internal offset: " << internalOffset << " and size: " << KIND_GetSize(type) << endl; 
        cout << "\t\tVALUE returned (float): " << value.as_flp << endl;
        cout << "\t\tVALUE returned (int): " << value.as_int << endl;
      }

      // copying src into dest
      srcLocation->copy(destLocation);
      destLocation->setSize(KIND_GetSize(type));
      destLocation->setValue(value);
      destLocation->setType(type);

      // sync load
      syncLoad(destLocation, src, type);
    } else {

      if (debug) cout << "\tSource pointer is not initialized!" << endl;

      VALUE zeroValue;
      zeroValue.as_int = 0;

      destLocation->setSize(KIND_GetSize(type));
      destLocation->setType(type);
      destLocation->setValue(zeroValue);
      destLocation->setLength(0);

      // sync load
      syncLoad(destLocation, src, type);
    }

    destLocation->setLineNumber(line);

    executionStack.top()[inx] = destLocation;
    if (debug) {
      cout << "stack frame index " << inx << endl; 
      cout << destLocation->toString() << endl;
      cout << destLocation << endl; 
    }
  }
  else {
    // NEW case for pointer constants
    // TODO: revise again
    if (debug)
      cout << "[Load] => pointer constant" << endl;

    IValue* destLocation = new IValue();
    destLocation->setSize(KIND_GetSize(type));
    destLocation->setType(type);
    destLocation->setLength(0);

    // sync load
    syncLoad(destLocation, src, type);

    destLocation->setLineNumber(line);
    executionStack.top()[inx] = destLocation;
    if (debug) {
      cout << destLocation->toString() << endl;
      cout << destLocation << endl; 
    }
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
  if (kind == INT1_KIND || kind == INT8_KIND || kind == INT16_KIND || kind == INT24_KIND || kind == INT32_KIND || kind == INT64_KIND) {
    return op->value.as_int;
  } else if (kind == INT80_KIND) {
    cout << "[getValueFromConstant] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return 0; // otherwise compiler warning
  } else {
    return op->value.as_flp;
  }
}

long double InterpreterObserver::getValueFromIValue(IValue* loc) {
  KIND kind = loc->getType();
  if (kind == INT1_KIND || kind == INT8_KIND || kind == INT16_KIND || kind == INT24_KIND || kind == INT32_KIND || kind == INT64_KIND) {
    return loc->getValue().as_int;
  } else if (kind == INT80_KIND) {
    cout << "[getValueFromIValue] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return 0; // otherwise compiler warning
  } else {
    return loc->getValue().as_flp;
  }
}

void InterpreterObserver::binop(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx, BINOP op) {
  if (debug) {
    printf("<<<<< %s >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", 
        BINOP_ToString(op).c_str(),
        IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(),
        inx);
  }

  if (op1->kind == INT80_KIND || op2->kind == INT80_KIND) {
    cout << "[getValueFromConstant] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  int64_t v1, v2;
  long double d1, d2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = op1->value.as_int;
    d1 = op1->value.as_flp;
  } else { // register
    IValue *loc1 = executionStack.top()[op1->inx];
    v1 = loc1->getIntValue();
    d1 = loc1->getFlpValue();
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = op2->value.as_int;
    d2 = op2->value.as_flp;
  } else { // register
    IValue *loc2 = executionStack.top()[op2->inx];
    v2 = loc2->getIntValue();
    d2 = loc2->getFlpValue();
  }

  VALUE vresult;
  if (debug) {
    cout << (int64_t)v1 << " " << (int64_t)v2 << endl;
    cout << (uint64_t)v1 << " " << (uint64_t)v2 << endl;
    cout << (double)d1 << " " << (double)d2 << endl;
  }
  switch (op) {
    case ADD:
      vresult.as_int = (int64_t) v1 + (int64_t) v2;
      break;
    case SUB:
      vresult.as_int = (int64_t) v1 - (int64_t) v2;
      break;
    case MUL:
      vresult.as_int = (int64_t) v1 * (int64_t) v2;
      break;
    case UDIV:
      vresult.as_int = (uint64_t) v1 / (uint64_t) v2;
      break;
    case SDIV:
      vresult.as_int = (int64_t) v1 / (int64_t) v2;
      break;
    case UREM:
      vresult.as_int = (uint64_t) v1 % (uint64_t) v2;
      break;
    case SREM:
      vresult.as_int = (int64_t) v1 % (int64_t) v2;
      break;
    case FADD:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) d1 + (float) d2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) d1 + (double) d2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = d1 + d2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FSUB:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) d1 - (float) d2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) d1 - (double) d2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = d1 - d2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FMUL:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) d1 * (float) d2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) d1 * (double) d2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = d1 * d2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FDIV:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) d1 / (float) d2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) d1 / (double) d2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = d1 / d2;
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
  if (debug) {
    cout << nloc->toString() << "\n";
    cout << nloc << endl;
  }

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
  if (debug) {
    printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  if (op1->kind == INT80_KIND || op2->kind == INT80_KIND) {
    cout << "[shl] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  uint64_t value1, value2;
  if (op1->inx == -1) {
    value1 = KVALUE_ToIntValue(op1);
  }
  else {
    IValue *loc1 = op1->isGlobal ? globalSymbolTable[op1->inx] : executionStack.top()[op1->inx];
    value1 = loc1->getIntValue();
  }

  if (op2->inx == -1) {
    value2 = KVALUE_ToIntValue(op2);
  }
  else {
    IValue *loc2 = op2->isGlobal ? globalSymbolTable[op2->inx] : executionStack.top()[op2->inx];
    value2 = loc2->getIntValue();
  }

  uint64_t result;
  switch (op1->kind) {
    case INT8_KIND:
      result = (uint8_t)value1 << (uint8_t)value2;
      break;
    case INT16_KIND:
      result = (uint16_t)value1 << (uint16_t)value2;
      break;
    case INT24_KIND:
    case INT32_KIND:
      result = (uint32_t)value1 << (uint32_t) value2;
      break;
    case INT64_KIND:
      result = value1 << value2;
      break;
    default:
      cout << "[SHL]: Operand type is not int8 or int16 or in24 or int32 or int64" << endl;
      safe_assert(false);
  }

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  nloc->setSize(KIND_GetSize(op1->kind));
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << value1 << " " << value2 << endl;
    cout << nloc->toString() << endl;
  }

  return;
}

void InterpreterObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug) {
    printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  // TODO: FIX THIS
  // ADD CASES FOR GLOBAL VARIABLES
  
  uint64_t value1, value2;
  if (op1->inx == -1) {
    value1 = KVALUE_ToIntValue(op1);
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


  uint64_t result;
  switch(op1->kind) {
  case INT8_KIND:
    result = (uint8_t)value1 >> (uint8_t)value2;
    break;
  case INT24_KIND:
  case INT32_KIND:
    result = (uint32_t)value1 >> (uint32_t)value2;
    break;
  case INT64_KIND:
    result = value1 >> value2;
    break;
  default:
    cout << "type: " << KIND_ToString(op1->kind) << endl;
    cout << "[LSHR]: Unhandled integer type" << endl;
    safe_assert(false);
  }

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  nloc->setSize(KIND_GetSize(op1->kind));
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << value1 << " " << value2 << endl;
    cout << nloc->toString() << endl;
  }

  return;
}

void InterpreterObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug) {
    printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  if (op1->kind == INT80_KIND || op2->kind == INT80_KIND) {
    cout << "[ashr] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  uint64_t value1, value2;
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


  uint64_t result;
  if (op1->kind == INT64_KIND) {
    result = value1 >> value2;
  }
  else if (op1->kind == INT32_KIND) {
    result = (uint32_t)value1 >> (uint32_t)value2;
  }
  else {
    cout << "[ASHR]: Operand type is not int32 or int64" << endl;
    safe_assert(false);
  }

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  nloc->setSize(KIND_GetSize(op1->kind));
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << value1 << " " << value2 << endl;
    cout << nloc->toString() << endl;
  }
  return;
}

void InterpreterObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug) {
    printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  if (op1->kind == INT80_KIND || op2->kind == INT80_KIND) {
    cout << "[and_] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  int64_t value1, value2;
  if (op1->inx == -1) {
    value1 = KVALUE_ToIntValue(op1);
  }
  else {
    IValue *loc1 = op1->isGlobal? globalSymbolTable[op1->inx] : executionStack.top()[op1->inx];
    value1 = loc1->getIntValue();
  }

  if (op2->inx == -1) {
    value2 = KVALUE_ToIntValue(op2);
  }
  else {
    IValue *loc2 = op2->isGlobal? globalSymbolTable[op2->inx] : executionStack.top()[op2->inx];
    value2 = loc2->getIntValue();
  }


  int64_t result;
  switch (op1->kind) {
    case INT1_KIND:
      result = (value1 == 1 && value2 == 1) ? 1 : 0;
      break;
    case INT8_KIND:
      if (debug) cout << "Value 1: " << (int8_t) value1 << " Value 2: " << (int8_t) value2 << endl;
      result = (int8_t)value1 & (int8_t)value2;
      break;
    case INT16_KIND:
      if (debug) cout << "Value 1: " << (int16_t) value1 << " Value 2: " << (int16_t) value2 << endl;
      result = (int16_t)value1 & (int16_t)value2;
      break;
    case INT24_KIND:
    case INT32_KIND:
      if (debug) cout << "Value 1: " << (int32_t) value1 << " Value 2: " << (int32_t) value2 << endl;
      result = (int32_t)value1 & (int32_t)value2;
      break;
    case INT64_KIND:
      if (debug) cout << "Value 1: " << (int64_t) value1 << " Value 2: " << (int64_t) value2 << endl;
      result = value1 & value2;
      break;
    default:
      cout << "[AND]: Operand type is not int8-64" << endl;
      cout << KIND_ToString(op1->kind) << endl;
      safe_assert(false);
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

void InterpreterObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug) {
    printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  if (op1->kind == INT80_KIND || op2->kind == INT80_KIND) {
    cout << "[or_] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  int64_t value1, value2;
  if (op1->inx == -1) {
    value1 = KVALUE_ToIntValue(op1);
  }
  else {
    IValue *loc1 = op1->isGlobal? globalSymbolTable[op1->inx] : executionStack.top()[op1->inx];
    value1 = loc1->getIntValue();
  }

  if (op2->inx == -1) {
    value2 = KVALUE_ToIntValue(op2);
  }
  else {
    IValue *loc2 = op2->isGlobal? globalSymbolTable[op2->inx] : executionStack.top()[op2->inx];
    value2 = loc2->getIntValue();
  }

  int64_t result;
  switch (op1->kind) {
    case INT1_KIND:
      result = (value1 == 0 && value2 == 0) ? 0 : 1;
      break;
    case INT8_KIND:
      result = (int8_t)value1 | (int8_t)value2;
      break;
    case INT16_KIND:
      result = (int16_t)value1 | (int16_t)value2;
      break;
    case INT24_KIND:
    case INT32_KIND:
      result = (int32_t)value1 | (int32_t)value2;
      break;
    case INT64_KIND:
      result = value1 | value2;
      break;
    default:
      cout << "[OR]: Operand type is not int8-64" << endl;
      cout << KIND_ToString(op1->kind) << endl;
      safe_assert(false);
  }

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  nloc->setSize(KIND_GetSize(op1->kind));
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << value1 << " " << value2 << endl;
    cout << nloc->toString() << endl;
  }
  return;
}

void InterpreterObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug) {
    cout << "========" << endl;
    printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (nuw ? "1" : "0"),
        (nsw ? "1" : "0"),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  if (op1->kind == INT80_KIND || op2->kind == INT80_KIND) {
    cout << "[xor_] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  int64_t value1, value2;
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

  int64_t result;
  if (op1->kind == INT64_KIND) {
    result = value1 ^ value2;
  }
  else if (op1->kind == INT32_KIND) {
    result = (int32_t)value1 ^ (int32_t)value2;
  }
  else {
    result = (int32_t)value1 ^ (int32_t)value2;
    //cout << "[XOR]: Operand type is not int32 or int64" << endl;
    //safe_assert(false);
  }

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(op1->kind, vresult);
  nloc->setSize(KIND_GetSize(op1->kind));
  executionStack.top()[inx] = nloc;

  if (debug) {
    cout << value1 << " " << value2 << endl;
    cout << nloc->toString() << endl;
  }
  return;
}

// ***** Vector Operations ***** //

void InterpreterObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug) {
    printf("<<<<< EXTRACTELEMENT >>>>> %s, vector:%s, index:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  cerr << "[InterpreterObserver::extractelement] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::insertelement() {
  if (debug) {
    printf("<<<<< INSERTELEMENT >>>>>\n");
  }

  cerr << "[InterpreterObserver::insertelement] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::shufflevector() {
  if (debug) {
    printf("<<<<< SHUFFLEVECTOR >>>>>\n");
  }

  cerr << "[InterpreterObserver::shufflevector] => Unimplemented\n";
  safe_assert(false);
}


// ***** AGGREGATE OPERATIONS ***** //

void InterpreterObserver::extractvalue(IID iid, int inx, int opinx) {
  if (debug) {
    printf("<<<<< EXTRACTVALUE >>>>> %s, agg_inx:%d, [INX: %d]\n",
        IID_ToString(iid).c_str(), opinx, inx);
  }

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

  if (debug) {
    cout << "KVALUE: " << KVALUE_ToString(aggKValue) << endl;
  }

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
  if (debug) {
    cout << evValue->toString() << "\n";
  }
  return;
}

void InterpreterObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  if (debug) {
    printf("<<<<< INSERTVALUE >>>>> %s, vector:%s, value:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(),
        KVALUE_ToString(op2).c_str(), inx);
  }

  cerr << "[InterpreterObserver::insertvalue] => Unimplemented\n";
  safe_assert(false);
}

// ***** Memory Access and Addressing Operations ***** //

void InterpreterObserver::allocax(IID iid, KIND type, uint64_t size, int inx, int line, bool arg, KVALUE* actualAddress) {
  if (debug) {
    printf("<<<<< ALLOCA >>>>> %s, kind:%s, size:%ld, arg: %d, line: %d, [INX: %d], %s\n", 
       IID_ToString(iid).c_str(), KIND_ToString(type).c_str(), size, arg, line, inx, KVALUE_ToString(actualAddress).c_str());
  }

  IValue *ptrLocation, *location;

  if (debug) {
    cout << "LOCAL alloca" << endl;
  }

  // alloca for non-argument variables
  location = new IValue(type); // should we count it as LOCAL?
  location->setLength(0);

  VALUE value;
  value.as_ptr = actualAddress->value.as_ptr;
  ptrLocation = new IValue(PTR_KIND, value, LOCAL);
  ptrLocation->setValueOffset((int64_t)location - (int64_t)value.as_ptr);
  if (debug) {
    cout << "actual address: " << actualAddress->value.as_ptr << endl;
    cout << "location" << location << endl;
  }

  ptrLocation->setSize(KIND_GetSize(type)); // put in constructor
  ptrLocation->setLineNumber(line);
  executionStack.top()[inx] = ptrLocation;

  if (debug) {
    cout << "Location: " << location->toString() << endl;
    cout << ptrLocation->toString() << endl;
  }

  safe_assert(ptrLocation->getValueOffset() != -1);
  return;
}

void InterpreterObserver::allocax_array(IID iid, KIND type, uint64_t size, int inx, int line, bool arg, KVALUE* addr) {
  if (debug) {
    printf("<<<<< ALLOCA_ARRAY >>>>> %s, elemkind:%s, arg: %d, line: %d, addr: %s, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(type).c_str(), arg, line, KVALUE_ToString(addr).c_str(), inx);
  }

  unsigned firstByte = 0;
  unsigned bitOffset = 0;
  unsigned length = 0; 

  //
  // calculate size of the array to be allocated
  //
  size = 1;
  while (!arraySize.empty()) {
    size = size * arraySize.front();
    arraySize.pop();
  }

  //
  // if array element is struct, get list of primitive types for each struct
  // element
  //
  uint64_t structSize = 1;
  if (type == STRUCT_KIND) {
   structSize = structType.size(); 
  }
  KIND* structKind = (KIND*) malloc(structSize*sizeof(KIND));
  if (type == STRUCT_KIND) {
    for (uint64_t i = 0; i < structSize; i++) {
      structKind[i] = structType.front();
      structType.pop();
    }
  }

  IValue* locArr = (IValue*) malloc(size*structSize*sizeof(IValue));
  for (uint64_t i = 0; i < size; i++) {
    if (type == STRUCT_KIND) {
      for (uint64_t j = 0; j < structSize; j++) {
        IValue* var = new IValue(structKind[j]);
        length++;
        var->setFirstByte(firstByte + bitOffset/8);
        var->setBitOffset(bitOffset%8);
        var->setLength(0);
        unsigned structType = structKind[j];
        firstByte += KIND_GetSize(structType);
        bitOffset = (structType == INT1_KIND) ? bitOffset + 1 : bitOffset;
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

  IValue* locArrPtr = new IValue(PTR_KIND, addr->value, LOCAL);
  locArrPtr->setValueOffset((int64_t)locArr - (int64_t)locArrPtr->getPtrValue());
  locArrPtr->setSize(KIND_GetSize(locArr[0].getType()));
  locArrPtr->setLength(length);
  locArrPtr->setLineNumber(line);
  executionStack.top()[inx] = locArrPtr;

  if (debug) {
    cout << executionStack.top()[inx]->toString() << endl;
  }

  safe_assert(locArrPtr->getValueOffset() != -1);
  return;
}

void InterpreterObserver::allocax_struct(IID iid, uint64_t size, int inx, int line, bool arg, KVALUE* addr) {
  if (debug) {
    printf("<<<<< ALLOCA STRUCT >>>>> %s, size: %ld, arg: %d, line: %d, addr: %s, [INX: %d]\n", IID_ToString(iid).c_str(), size, arg, line, KVALUE_ToString(addr).c_str(), inx);
  }

  safe_assert(structType.size() == size);

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

  IValue* structPtrVar = new IValue(PTR_KIND, addr->value);
  structPtrVar->setValueOffset((int64_t) ptrToStructVar - (int64_t) structPtrVar->getPtrValue());
  structPtrVar->setSize(KIND_GetSize(ptrToStructVar[0].getType()));
  structPtrVar->setLength(length);
  structPtrVar->setLineNumber(line);

  executionStack.top()[inx] = structPtrVar;

  if (debug) {
    cout << executionStack.top()[inx]->toString() << endl;
  }

  safe_assert(structPtrVar->getValueOffset() != -1);
  return;
}


bool InterpreterObserver::checkStore(IValue *dest, KVALUE *kv) {
  bool result = false;

  switch(kv->kind) {
    case PTR_KIND:
      cout << "index: " << dest->getIndex() << endl;
      cout << "size: " << dest->getSize() << endl;
      cout << "value: " << dest->getValue().as_int << endl;
      cout << "first value: " << dest->getValue().as_int + dest->getOffset() << endl;
      cout << "offset: " << dest->getOffset() << endl;
      cout << "valueOffset: " << dest->getValueOffset() << endl;
      cout << "bitoffset: " << dest->getBitOffset() << endl;
      cout << "second value: " << kv->value.as_int << endl;

      result = ((int64_t)dest->getValue().as_ptr + dest->getOffset() == (int64_t)kv->value.as_ptr);
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
    case INT24_KIND:
      result = (dest->getIntValue() == KVALUE_ToIntValue(kv));
      break;
    case INT32_KIND: 
      result = ((int32_t)dest->getValue().as_int == (int32_t)kv->value.as_int);
      break;
    case INT64_KIND:
      if (debug) {
        cout << dest->getValue().as_int << " " << kv->value.as_int << endl;
      }
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
  if (debug) {
    printf("<<<<< STORE >>>>> %s, %s, %s, line: %d, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(dest).c_str(), 
        KVALUE_ToString(src).c_str(), line, inx); 
  }

  if (dest->kind == INT80_KIND || src->kind == INT80_KIND) {
    cout << "[store] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  // pointer constant; we simply ignore this case
  if (dest->inx == -1) {
    if (debug) cout << "\tIgnoring pointer constant." << endl;
    return; 
  }

  // retrieve destination pointer operand
  IValue* destPtrLocation = dest->isGlobal? globalSymbolTable[dest->inx] :
    executionStack.top()[dest->inx];

  if (debug) cout << "\tDestPtr: " << destPtrLocation->toString() << endl;

  // the destination pointer is not initialized
  // initialize with an empty IValue object
  if (!destPtrLocation->isInitialized()) {
    if (debug) {
      cout << "\tDestination pointer location is not initialized" << endl;
    }
    IValue* iValue = new IValue(src->kind);
    iValue->setLength(0);
    destPtrLocation->setValueOffset( (int64_t)iValue - (int64_t)destPtrLocation->getPtrValue() ); 
    destPtrLocation->setInitialized();
    if (debug) cout << "\tInitialized destPtr: " << destPtrLocation->toString() << endl;
  }

  unsigned destPtrOffset = destPtrLocation->getOffset();
  IValue *destLocation = NULL;
  IValue *srcLocation = NULL;
  int internalOffset = 0;

  // retrieve source
  if (src->iid == 0) {
    srcLocation = new IValue(src->kind, src->value);
    srcLocation->setLength(0); // uninitialized constant pointer 
    if (src->kind == INT1_KIND) {
      srcLocation->setBitOffset(1);
    }
  } else {
    if (src->isGlobal) {
      srcLocation = globalSymbolTable[src->inx];
    } else {
      srcLocation = executionStack.top()[src->inx];
    }
  }

  if (debug) {
    cout << "\tSrc: " << srcLocation->toString() << endl;
  }

  // retrieve actual destination
  IValue* values = (IValue*)destPtrLocation->getIPtrValue();
  unsigned valueIndex = destPtrLocation->getIndex();
  unsigned currOffset = values[valueIndex].getFirstByte();
  destLocation = values + valueIndex;
  internalOffset = destPtrOffset - currOffset;

  if (debug) {
    cout << "\tdestPtrOffset: " << destPtrOffset << endl;

    cout << "\tvalueIndex: " << valueIndex << " currOffset: " << currOffset << " Other offset: "  << destPtrOffset << endl;

    cout << "\tinternalOffset: " << internalOffset <<  " Size: " << destPtrLocation->getSize() << endl;

    cout << "\tDest: " << destLocation->toString() << endl;

    cout << "\tCalling writeValue with offset: " << internalOffset << ", size: " << destPtrLocation->getSize() << endl;
  }

  // writing src into dest
  destPtrLocation->writeValue(internalOffset, KIND_GetSize(src->kind), srcLocation);
  destPtrLocation->setInitialized();

  if (debug) {
    cout << "\tUpdated Dest: " << destLocation->toString() << endl;
  }

  // just read again to check store
  if (debug) {
    cout << "\tCalling readValue with internal offset: " << internalOffset << " size: " << destPtrLocation->getSize() << endl;
  }

  // NOTE: destLocation->getType() before
  IValue* writtenValue = new IValue(srcLocation->getType(),
				    destPtrLocation->readValue(internalOffset, src->kind)); 
  writtenValue->setSize(destLocation->getSize());
  writtenValue->setIndex(destLocation->getIndex());
  writtenValue->setOffset(destLocation->getOffset());
  writtenValue->setBitOffset(destLocation->getBitOffset());

  if (debug) {
    cout << "\twrittenValue: " << writtenValue->toString() << endl;
  }

  if (!checkStore(writtenValue, src)) { // destLocation
    cout << "\tKVALUE: " << KVALUE_ToString(src) << endl;
    cout << "\tMismatched values found in Store" << endl;
    safe_assert(false);
  }

  if (debug) {
    cout << "\tsrcLocation: " << srcLocation->toString() << endl;
  }

  return;
}

void InterpreterObserver::fence() {
  if (debug) {
    printf("<<<<< FENCE >>>>>\n");
  }

  cerr << "[InterpreterObserver::fence] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx) {
  if (debug) {
    printf("<<<<< CMPXCHG >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	   PTR_ToString(addr).c_str(),
	   KVALUE_ToString(kv1).c_str(),
	   KVALUE_ToString(kv2).c_str(), 
	   inx);
  }

  cerr << "[InterpreterObserver::cmpxchg] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::atomicrmw() {
  if (debug) {
    printf("<<<<< ATOMICRMW >>>>>\n");
  }

  cerr << "[InterpreterObserver::atomicrmw] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::getelementptr(IID iid, bool inbound, KVALUE* base, KVALUE* offset, KIND type, uint64_t size, int line, int inx) {
  if (debug) {
    printf("<<<<< GETELEMENTPTR= >>>>> %s, inbound:%s, pointer_value:%s, index:%s, kind: %s, size: %ld, line: %d, [INX: %d]\n\n", 
        IID_ToString(iid).c_str(),
        (inbound ? "1" : "0"),
        KVALUE_ToString(base).c_str(),
        KVALUE_ToString(offset).c_str(),
        KIND_ToString(type).c_str(),
        size,
        line,
        inx);
  }

  if (type == INT80_KIND) {
    cout << "[getelementptr] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }


  IValue *basePtrLocation, *ptrLocation; 
  int index;
  int newOffset;

  //
  // get base pointer operand
  //
  if (base->inx == -1) {
    // constant base pointer
    basePtrLocation = new IValue(PTR_KIND, base->value, 0, 0, 0, 0);
  } else {
    basePtrLocation = base->isGlobal ? globalSymbolTable[base->inx] :
      executionStack.top()[base->inx];
  }

  if (debug) cout << "\tPointer operand " << basePtrLocation->toString() << endl;

  //
  // get index operand
  // 
  index = offset->inx == -1 ? offset->value.as_int :
    executionStack.top()[offset->inx]->getValue().as_int; 

  //
  // compute new offset from base pointer in bytes 
  // 
  newOffset = (index * (size/8)) + basePtrLocation->getOffset();

  if (debug) {
    cout << "\tSize: " << size << endl;
    cout << "\tBase Offset: " << basePtrLocation->getOffset() << endl;
    cout << "\tIndex: " << index << endl;
    cout << "\tnewOffset: " << newOffset << endl;
  }

  if (basePtrLocation->isInitialized()) {
    unsigned index = findIndex((IValue*) basePtrLocation->getIPtrValue(), newOffset, basePtrLocation->getLength()); // TODO: revise offset, getValue().as_ptr
    ptrLocation = new IValue(PTR_KIND, basePtrLocation->getValue(), size/8, newOffset, index, basePtrLocation->getLength());
  } else {
    cout << "\tPointer is not initialized!" << endl;
    VALUE newPtrValue;
    newPtrValue.as_int = basePtrLocation->getValue().as_int + newOffset;
    ptrLocation = new IValue(PTR_KIND, newPtrValue, size/8, 0, 0, 0);
  }

  ptrLocation->setValueOffset(basePtrLocation->getValueOffset());
  ptrLocation->setLineNumber(line);

  executionStack.top()[inx] = ptrLocation;
  if (debug) {
    cout << executionStack.top()[inx]->toString() << endl;
  }
  //safe_assert(ptrLocation->getValueOffset() != -1);
  return;
}

void InterpreterObserver::getelementptr_array(IID iid, bool inbound, KVALUE* op, KIND kind, int elementSize, int inx) {
  if (debug)
    printf("<<<<< GETELEMENTPTR_ARRAY >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, elementSize: %d, [INX: %d]\n", 
        IID_ToString(iid).c_str(),
        (inbound ? "1" : "0"),
        KVALUE_ToString(op).c_str(),
        KIND_ToString(kind).c_str(),
        elementSize,
        inx);

  IValue* arrayElemPtr;
  int elemFlattenSize, newOffset;

  elemFlattenSize = 0;
  while (!structElementSize.empty()) {
    elemFlattenSize += structElementSize.front();
    structElementSize.pop();
  }
  elemFlattenSize = elemFlattenSize == 0 ? 1 : elemFlattenSize;
  if (debug) cout << "\tStruct element flatten size: " << elemFlattenSize << endl; 

  if (op->iid == 0) {
    // TODO: review this
    // constant pointer
    // return a dummy object
    arrayElemPtr = new IValue(PTR_KIND, op->value, 0, 0, 0, 0);
    getElementPtrIndexList.pop();
    if (!getElementPtrIndexList.empty()) {
      getElementPtrIndexList.pop();
      safe_assert(getElementPtrIndexList.empty());
    }
  } else {
    IValue *ptrArray, *array;
    int *arraySizeVec, *indexVec;
    int index, arrayDim, getIndexNo, i, j;

    ptrArray = op->isGlobal ? globalSymbolTable[op->inx] : executionStack.top()[op->inx];
    array = static_cast<IValue*>(ptrArray->getIPtrValue());

    if (debug) cout << "\tPointer operand: " << ptrArray->toString() << endl;

    //
    // compute the index for flatten array representation of
    // the program's multi-dimensional array
    //
    
    arrayDim = arraySize.size();

    if (debug) cout << "arrayDim " << arrayDim << endl;

    arraySizeVec = (int*) malloc(arrayDim * sizeof(int));

    getIndexNo = getElementPtrIndexList.size();

    if (debug) cout << "getIndexNo " << getIndexNo << endl;

    indexVec = (int*) malloc(getIndexNo * sizeof(int));

    // the size of out-most dimension; 
    // we do not need this to compute the index
    safe_assert(!arraySize.empty());
    arraySize.pop(); 
    i = 0;
    while (!arraySize.empty()) {
      arraySizeVec[i] = arraySize.front();
      arraySize.pop();
      i++;
    }
    safe_assert(arraySize.empty());

    arraySizeVec[arrayDim-1] = 1; 

    for (i = 0; i < arrayDim - 1; i++) {
      for (j = i+1; j < getIndexNo - 1; j++) {
        arraySizeVec[i] *= arraySizeVec[j]; 
      }
    }

    // the first index is for the pointer operand;
    // it must be zero
    safe_assert(getElementPtrIndexList.front() == 0);
    getElementPtrIndexList.pop(); 

    i = 0;
    while (!getElementPtrIndexList.empty()) {
      indexVec[i] = getElementPtrIndexList.front();
      getElementPtrIndexList.pop();
      i++;
    }
    safe_assert(getElementPtrIndexList.empty());

    index = 0;
    for (i = 0; i < getIndexNo - 1; i++) {
      index += indexVec[i] * arraySizeVec[i];
    }

    if (debug) cout << "\tIndex: " << index << endl;

    //
    // compute new offset for flatten array
    //
    newOffset = ptrArray->getOffset() + elementSize*index; 

    //
    // compute the index for the casted fatten array
    //
    
    if (ptrArray->isInitialized()) {
      index = findIndex((IValue*) ptrArray->getIPtrValue(), newOffset, ptrArray->getLength()); 
    }
    
    if (debug) cout << "\tIndex: " << index << endl;

    // TODO: revisit this
    if (index < (int) ptrArray->getLength()) {
      IValue* arrayElem = array + index;
      arrayElemPtr = new IValue(PTR_KIND, ptrArray->getValue());
      arrayElemPtr->setValueOffset(ptrArray->getValueOffset());
      arrayElemPtr->setIndex(index);
      arrayElemPtr->setLength(ptrArray->getLength());
      arrayElemPtr->setSize(KIND_GetSize(arrayElem[0].getType()));
      arrayElemPtr->setOffset(arrayElem[0].getFirstByte());
    } else {
      VALUE arrayElemPtrValue;
      arrayElemPtrValue.as_int = ptrArray->getValue().as_int + newOffset;
      arrayElemPtr = new IValue(PTR_KIND, arrayElemPtrValue, ptrArray->getSize(), 0, 0, 0);
      arrayElemPtr->setValueOffset((int64_t)arrayElemPtr - arrayElemPtr->getValue().as_int);
    }
  }

  safe_assert(getElementPtrIndexList.empty());
  executionStack.top()[inx] = arrayElemPtr;
  if (debug)
    cout << executionStack.top()[inx]->toString() << endl;
}

void InterpreterObserver::getelementptr_struct(IID iid, bool inbound, KVALUE* op, KIND kind, KIND arrayKind, int inx) {
  if (debug)
    printf("<<<<< GETELEMENTPTR_STRUCT >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, arrayKind: %s, [INX: %d]\n\n", 
        IID_ToString(iid).c_str(),
        (inbound ? "1" : "0"),
        KVALUE_ToString(op).c_str(),
        KIND_ToString(kind).c_str(),
        KIND_ToString(arrayKind).c_str(),
        inx);

  if (debug) cout << "\tstructType size " << structType.size() << endl;

  IValue *structPtr, *structElemPtr; 
  int structElemNo, structSize, index, size, i, newOffset;
  int* structElemSize, *structElem;

  //
  // get the struct operand
  //
  structPtr = executionStack.top()[op->inx];
  structElemNo = structType.size();
  structElemSize = (int*) malloc(sizeof(int)*structElemNo);
  structElem = (int*) malloc(sizeof(int)*structElemNo);

  //
  // record struct element size
  // compute struct size
  //
  structSize = 0;
  i = 0;
  while (!structType.empty()) {
    structElemSize[i] = KIND_GetSize(structType.front());
    structElem[i] = structType.front();
    structSize += structElemSize[i];
    i++;
    structType.pop();
  }

  if (debug) cout << "\tstructSize is " << structSize << endl;

  if (debug) cout << "\t" << structPtr->toString() << endl;

  // compute struct index
  cout << "\tsize of getElementPtrIndexList: " << getElementPtrIndexList.size() << endl;
  index = getElementPtrIndexList.front()*structElemNo;
  getElementPtrIndexList.pop();
  if (!getElementPtrIndexList.empty()) {
    unsigned i;
    for (i = 0; i < getElementPtrIndexList.front(); i++) {
      index = index + structElementSize.front();
      safe_assert(!structElementSize.empty());
      structElementSize.pop();
    }
  }
  if (!getElementPtrIndexList.empty()) {
    getElementPtrIndexList.pop();
  }
  while (!structElementSize.empty()) {
    structElementSize.pop();
  }
  safe_assert(getElementPtrIndexList.empty());

  if (debug) cout << "\tIndex is " << index << endl;

  newOffset = structSize * (index/structElemNo);
  for (i = 0; i < index % structElemNo; i++) {
    newOffset = newOffset + KIND_GetSize(structElem[i]);
  }

  newOffset = newOffset + structPtr->getOffset();

  size = KIND_GetSize(structElem[index % structElemNo]);


  if (debug) cout << "\tNew offset is: " << newOffset << endl;

  //
  // compute the result; consider two cases: the struct pointer operand is
  // initialized and is not initialized
  //
  if (structPtr->isInitialized()) {
    IValue* structBase = static_cast<IValue*>(structPtr->getIPtrValue());


    index = findIndex((IValue*) structPtr->getIPtrValue(), newOffset, structPtr->getLength()); // TODO: revise offset, getValue().as_ptr

    if (debug) cout << "\tNew index is: " << index << endl;

    // TODO: revisit this
    if (index < (int) structPtr->getLength()) {
      if (debug) {
        cout << "\tstructBase = " << structBase->toString() << endl;
      }
      IValue* structElem = structBase + index;
      if (debug) {
        cout << "\tstructElem = " << structElem->toString() << endl;
      }
      structElemPtr = new IValue(PTR_KIND, structPtr->getValue());
      structElemPtr->setValueOffset(structPtr->getValueOffset());
      structElemPtr->setIndex(index);
      structElemPtr->setLength(structPtr->getLength());
      structElemPtr->setSize(size);
      structElemPtr->setOffset(newOffset);
    } else {
      structElemPtr = new IValue(PTR_KIND, structPtr->getValue(), structPtr->getSize(), 0, 0, 0);
      structElemPtr->setValueOffset(structPtr->getValueOffset());
    }
  } else {
    if (debug) cout << "\tPointer is not initialized" << endl;
    VALUE structElemPtrValue;

    // compute the value for the element pointer
    structElemPtrValue = structPtr->getValue();
    structElemPtrValue.as_int = structElemPtrValue.as_int + newOffset;

    structElemPtr = new IValue(PTR_KIND, structElemPtrValue, size, 0, 0, 0);
    structElemPtr->setValueOffset((int64_t)structElemPtr - structElemPtr->getValue().as_int);
  }

  executionStack.top()[inx] = structElemPtr;

  if (debug) {
    cout << executionStack.top()[inx]->toString() << endl;
  }
}

// ***** Conversion Operations ***** //

void InterpreterObserver::trunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< TRUNC >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);
  }

  IValue* src = op->isGlobal? globalSymbolTable[op->inx] : executionStack.top()[op->inx];
  VALUE value = src->getValue();
  int64_t intValue = value.as_int;
  int64_t* int64Ptr = &intValue;
  bool* int1Ptr = (bool*) int64Ptr;
  int8_t* int8Ptr = (int8_t*) int64Ptr;
  int16_t* int16Ptr = (int16_t*) int64Ptr;
  int32_t* int32Ptr = (int32_t*) int64Ptr;
  VALUE truncValue;
  IValue* srcTemp = new IValue();

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
    case INT24_KIND:
      src->copy(srcTemp);
      srcTemp->setType(INT24_KIND);
      truncValue.as_int = srcTemp->getIntValue();
      break;
    case INT32_KIND:
      truncValue.as_int = *int32Ptr;
      break;
    case INT64_KIND:
      truncValue.as_int = *int64Ptr;
      break;
    case INT80_KIND:
      cout << "[trunc] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      break;
    default:
      cerr << "[InterpreterObserver::trunc] => Unsupport integer type " << type << "\n";
      safe_assert(false);
  }

  IValue* truncVar = new IValue(type, truncValue);
  executionStack.top()[inx] = truncVar;
  if (debug)
    cout << executionStack.top()[inx]->toString() << endl;
}

void InterpreterObserver::zext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< ZEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);
  }

  IValue* src = op->isGlobal? globalSymbolTable[op->inx] : executionStack.top()[op->inx];

  if (debug) {
    cout << "Source value: " << src->toString() << endl;
  }
  int64_t intValue = src->getValue().as_int;
  int64_t* intValuePtr;
  intValuePtr = &intValue;

  if (op->kind == INT1_KIND) {
    intValue = (bool) intValue;
  }
  VALUE zextValue;
  IValue* srcTemp = new IValue();

  switch (type) {
    case INT1_KIND:
      zextValue.as_int = intValue & (1<<0); // TODO: confirm this
      break;
    case INT8_KIND:
      zextValue.as_int = *((int8_t*) intValuePtr);
      break;
    case INT16_KIND:
      zextValue.as_int = *((int16_t*) intValuePtr);
      break;
    case INT24_KIND:
      src->copy(srcTemp);
      srcTemp->setType(INT24_KIND);
      zextValue.as_int = srcTemp->getIntValue();
      break;
    case INT32_KIND: 
      zextValue.as_int = *((int32_t*) intValuePtr);
      break;
    case INT64_KIND:
      zextValue.as_int = *((int64_t*) intValuePtr);
      break;
    case INT80_KIND:
      cout << "[zext] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      break;
    default:
      cerr << "[InterpreterObserver::zext] => Unsupport integer type " << type << "\n";
      safe_assert(false);
  }

  IValue* zextVar = new IValue(type, zextValue);
  executionStack.top()[inx] = zextVar;
  if (debug) {
    cout << executionStack.top()[inx]->toString() << "\n";
  }
}

void InterpreterObserver::sext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< SEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);
  }

  IValue* src = op->isGlobal? globalSymbolTable[op->inx] : executionStack.top()[op->inx];
  VALUE value = src->getValue();

  VALUE ext_value;
  IValue* srcTemp = new IValue();

  switch (type) {
    case INT1_KIND:
      ext_value.as_int = value.as_int & (1<<0); // TODO: confirm this
      break;
    case INT8_KIND:
      ext_value.as_int = (int8_t) src->getIntValue();
      break;
    case INT16_KIND:
      ext_value.as_int = (int16_t) src->getIntValue();
      break;
    case INT24_KIND:
      src->copy(srcTemp);
      srcTemp->setType(INT24_KIND);
      ext_value.as_int = srcTemp->getIntValue();
      break;
    case INT32_KIND: 
      ext_value.as_int = (int32_t) src->getIntValue();
      break;
    case INT64_KIND:
      ext_value.as_int = (int64_t) src->getIntValue();
      break;
    case INT80_KIND:
      cout << "[sext] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      break;
    default:
      cerr << "[InterpreterObserver::sext] => Unsupport integer type " << type << "\n";
      safe_assert(false);
  }

  IValue *ext_loc = new IValue(type, ext_value);
  executionStack.top()[inx] = ext_loc;
  if (debug) {
    cout << ext_loc->toString() << endl;
  }
}

void InterpreterObserver::fptrunc(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< FPTRUNC >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);
  }

  IValue* src = kv->isGlobal? globalSymbolTable[kv->inx] : executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND) {
    trunc_value.as_flp = (float) value.as_flp;
  }
  else if (type == FLP64_KIND) {
    trunc_value.as_flp = (double) value.as_flp;
  }
  else if (type == FLP80X86_KIND) {
    trunc_value.as_flp = (long double) value.as_flp;
  }
  else {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    safe_assert(false);
  }

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug) {
    cout << trunc_loc->toString() << endl;
  }
}

void InterpreterObserver::fpext(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< FPEXT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);
  }

  IValue* src = kv->isGlobal? globalSymbolTable[kv->inx] : executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND) {
    trunc_value.as_flp = (float) value.as_flp;
  }
  else if (type == FLP64_KIND) {
    trunc_value.as_flp = (double) value.as_flp;
  }
  else if (type == FLP80X86_KIND) {
    trunc_value.as_flp = (long double) value.as_flp;
  }
  else {
    cerr << "[InterpreterObserver::fpext] => Unsupport floating point type " << type << "\n";
    safe_assert(false);
  }

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug) {
    cout << trunc_loc->toString() << endl;
  }
}

void InterpreterObserver::fptoui(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< FPTOUII >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);
  }

  if (type == INT80_KIND) {
      cout << "[fptoui] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      return;
  }

  IValue* src = kv->isGlobal? globalSymbolTable[kv->inx] : executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;

  if (debug) {
    cout << trunc_loc->toString() << endl;
  }
}

void InterpreterObserver::fptosi(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< FPTOSI >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);
  }

  if (type == INT80_KIND) {
    cout << "[fptosi] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return;
  }

  IValue* src = kv->isGlobal? globalSymbolTable[kv->inx] : executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;

  if (debug) {
    cout << trunc_loc->toString() << endl;
  }
}

void InterpreterObserver::uitofp(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< UITOFP >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);
  }

  IValue* src = kv->isGlobal? globalSymbolTable[kv->inx] : executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND) {
    trunc_value.as_flp = (float) value.as_int;
  }
  else if (type == FLP64_KIND) {
    trunc_value.as_flp = (double) value.as_int;
  }
  else if (type == FLP80X86_KIND) {
    trunc_value.as_flp = (long double) value.as_int;
  }
  else {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    safe_assert(false);
  }

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;
  if (debug) {
    cout << trunc_loc->toString() << endl;
  }
}

void InterpreterObserver::sitofp(IID iid, KIND type, KVALUE* kv, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< SITOFP >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(kv).c_str(), size, inx);
  }

  IValue* src = kv->isGlobal? globalSymbolTable[kv->inx] : executionStack.top()[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND) {
    trunc_value.as_flp = (float) value.as_int;
  }
  else if (type == FLP64_KIND) {
    trunc_value.as_flp = (double) value.as_int;
  }
  else if (type == FLP80X86_KIND) {
    trunc_value.as_flp = (long double) value.as_int;
  }
  else {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    safe_assert(false);
  }

  IValue *trunc_loc = new IValue(type, trunc_value);
  executionStack.top()[inx] = trunc_loc;

  if (debug) {
    cout << trunc_loc->toString() << endl;
  }
}

void InterpreterObserver::ptrtoint(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< PTRTOINT >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);
  }

  IValue* src = op->isGlobal? globalSymbolTable[op->inx] : executionStack.top()[op->inx];
  VALUE value = src->getValue();
  // to calculate the pointer value, need to add the offset
  int64_t ptrValue = value.as_int + src->getOffset(); 

  VALUE int_value;
  int8_t* ptrValue8;
  int64_t* ptrValue64;

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
    case INT24_KIND:
      ptrValue8 = (int8_t*) &ptrValue;
      ptrValue8[3] = 0;
      ptrValue8[4] = 0;
      ptrValue8[5] = 0;
      ptrValue64 = (int64_t*) ptrValue8;
      int_value.as_int = *ptrValue64;
      break;
    case INT32_KIND:
      int_value.as_int = (int32_t) ptrValue;
      break;
    case INT64_KIND:
      int_value.as_int = ptrValue;
      break;
    case INT80_KIND:
      cout << "[ptrtoint] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      break;
    default:
      safe_assert(false); // this cannot happen
  }

  IValue *ptrToInt = new IValue(type, int_value);
  executionStack.top()[inx] = ptrToInt;

  if (debug) {
    cout << executionStack.top()[inx]->toString() << endl;
  }
}

void InterpreterObserver::inttoptr(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< INTTOPTR >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);
  }

  if (op->kind == INT80_KIND) {
      cout << "[fptoui] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      return;
  }

  IValue *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();
  VALUE ptr_value;
  ptr_value.as_ptr = value.as_ptr;
  IValue *intToPtr = new IValue(type, ptr_value);
  executionStack.top()[inx] = intToPtr;

  if (debug) {
    cout << executionStack.top()[inx]->toString() << endl;
  }
}

void InterpreterObserver::bitcast(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  if (debug) {
    printf("<<<<< BITCAST >>>>> %s, %s, %s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(),
        KIND_ToString(type).c_str(), KVALUE_ToString(op).c_str(), size, inx);
  }

  if (op->kind == INT80_KIND || type == INT80_KIND) {
      cout << "[bitcast] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      return;
  }

  IValue *src = executionStack.top()[op->inx];
  VALUE value = src->getValue();

  IValue *bitcastLoc = new IValue(type, value, size/8, src->getOffset(), src->getIndex(), src->getLength()); // TODO: check
  bitcastLoc->setValueOffset(src->getValueOffset());
  executionStack.top()[inx] = bitcastLoc;

  if (debug) {
    cout << bitcastLoc->toString() << endl;
  }
  return;
}

// ***** TerminatorInst ***** //
void InterpreterObserver::branch(IID iid, bool conditional, KVALUE* op1, int inx) {
  if (debug) {
    printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (conditional ? "1" : "0"),
        KVALUE_ToString(op1).c_str(), inx);
  }

  IValue* cond = (op1->inx == -1) ? NULL : executionStack.top()[op1->inx];

  if (cond != NULL && ((bool) cond->getIntValue() != (bool) op1->value.as_int)) {
    if (debug) {
      cerr << "\tKVALUE: " << KVALUE_ToString(op1) << endl;
      cerr << "\tIVALUE: " << cond->toString() << endl;

      cerr << "\tShadow and concrete executions diverge at this branch." << endl;
    }
    safe_assert(false);
  }
}

void InterpreterObserver::branch2(IID iid, bool conditional, int inx) {
  if (debug) {
    printf("<<<<< BRANCH2 (GOTO) >>>>> %s, cond: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        (conditional ? "1" : "0"), inx);
  }
}

void InterpreterObserver::indirectbr(IID iid, KVALUE* op1, int inx) {
  if (debug) {
    printf("<<<<< INDIRECTBR >>>>> %s, address: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(), inx);
  }
}

void InterpreterObserver::invoke(IID iid, KVALUE* call_value, int inx) {
  if (debug) {
    printf("<<<<< INVOKE >>>>> %s, call_value: %s, [INX: %d]", IID_ToString(iid).c_str(), KVALUE_ToString(call_value).c_str(), inx);
  }

  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    if (debug)
      printf(", arg: %s", KVALUE_ToString(value).c_str()); 
  }
  if (debug) {
    cout << endl;
  }

  cerr << "[InterpreterObserver::invoke] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::resume(IID iid, KVALUE* op1, int inx) {
  if (debug) {
    printf("<<<<< RESUME >>>>> %s, acc_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	   KVALUE_ToString(op1).c_str(), inx);
  }

  cerr << "[InterpreterObserver::resume] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::return_(IID iid, KVALUE* op1, int inx) {
  if (debug) {
    printf("<<<<< RETURN 1>>>>> %s, ret_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op1).c_str(), inx);
  }

  safe_assert(!executionStack.empty());

  IValue* returnValue = op1->inx == -1 ? NULL : executionStack.top()[op1->inx];

  executionStack.pop();

  if (!executionStack.empty()) {
    if (debug) {
      cout << "New stack size: " << executionStack.size() << endl;
    }
    safe_assert(!callerVarIndex.empty());

    if (returnValue == NULL) {
      executionStack.top()[callerVarIndex.top()]->setValue(op1->value); 
      executionStack.top()[callerVarIndex.top()]->setType(op1->kind); 
    } else {
      returnValue->copy(executionStack.top()[callerVarIndex.top()]);
    }
    if (debug) {
      cout << executionStack.top()[callerVarIndex.top()]->toString() << endl;
    }

    callerVarIndex.pop();
  } else {
    cout << "The execution stack is empty.\n";
  }

  isReturn = true;

  return;
}

void InterpreterObserver::return2_(IID iid, int inx) {
  if (debug) {
    printf("<<<<< RETURN 2>>>>> %s, [INX: %d]\n", IID_ToString(iid).c_str(), inx);
  }

  safe_assert(!executionStack.empty());
  executionStack.pop();

  if (!executionStack.empty()) {
    if (debug) {
      cout << "New stack size: " << executionStack.size() << endl;
    }
  } else {
    cout << "The execution stack is empty.\n";
  }

  isReturn = true;

  return;
}

void InterpreterObserver::return_struct_(IID iid, int inx, int valInx) {
  if (debug) {
    printf("<<<<< RETURN STRUCT >>>>> %s, val_inx: %d, [INX: %d]\n", IID_ToString(iid).c_str(), valInx, inx);
  }

  safe_assert(!executionStack.empty());

  IValue* returnValue = (valInx == -1) ? NULL : executionStack.top()[valInx];

  executionStack.pop();

  if (!executionStack.empty()) {
    if (debug) {
      cout << "New stack size: " << executionStack.size() << endl;
    }
    safe_assert(!callerVarIndex.empty());
    safe_assert(!returnStruct.empty());

    // reconstruct struct value
    unsigned size = returnStruct.size();
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
      if (debug)
        cout << structValue[i].toString() << endl;
      i++;
      returnStruct.pop();
    }

    safe_assert(returnStruct.empty());

    executionStack.top()[callerVarIndex.top()] = structValue;
    if (debug) {
      for (i = 0; i < size; i++)
        cout << executionStack.top()[callerVarIndex.top()][i].toString() << endl;
    }
  } else {
    cout << "The execution stack is empty.\n";
  }

  safe_assert(!callerVarIndex.empty());
  callerVarIndex.pop();

  isReturn = true;

  return;
}

void InterpreterObserver::switch_(IID iid, KVALUE* op, int inx) {
  if (debug) {
    printf("<<<<< SWITCH >>>>> %s, condition: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(op).c_str(), inx);
  }
}

void InterpreterObserver::unreachable() {
  if (debug) {
    printf("<<<<< UNREACHABLE >>>>>\n");
  }
}

// ***** Other Operations ***** //

void InterpreterObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  if (debug) {
    printf("<<<<< ICMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(op1).c_str(), KVALUE_ToString(op2).c_str(), pred, inx);
  }

  if (op1->kind == INT80_KIND || op2->kind == INT80_KIND) {
      cout << "[icmp] Unsupported INT80_KIND" << endl;
      safe_assert(false);
      return;
  }

  int64_t v1, v2;

  // get value of v1
  if (op1->inx == -1) { // constant
    v1 = op1->value.as_int;
  } else {
    IValue *loc1 = op1->isGlobal ? globalSymbolTable[op1->inx] :
      executionStack.top()[op1->inx];
    v1 = loc1->getType() == PTR_KIND ? loc1->getIntValue() + loc1->getOffset()
      : loc1->getIntValue();
  }

  // get value of v2
  if (op2->inx == -1) { // constant
    v2 = op2->value.as_int;
  } else {
    IValue *loc2 = op2->isGlobal ? globalSymbolTable[op2->inx] :
      executionStack.top()[op2->inx];
    v2 = loc2->getType() == PTR_KIND ? loc2->getIntValue() + loc2->getOffset()
      : loc2->getIntValue();
  } 

  if (debug) {
    cout << "=============" << v1 << endl;
    cout << "=============" << v2 << endl;
  }

  int result = 0;
  switch(pred) {
    case CmpInst::ICMP_EQ:
      if(debug) cout << "PRED = ICMP_EQ" << endl;
      result = v1 == v2;
      break;
    case CmpInst::ICMP_NE:
      if(debug) cout << "PRED = ICMP_NE" << endl;
      result = v1 != v2;
      break;
    case CmpInst::ICMP_UGT:
      if(debug) cout << "PRED = ICMP_UGT" << endl;
      result = (uint64_t)v1 > (uint64_t)v2;
      break;
    case CmpInst::ICMP_UGE:
      if(debug) cout << "PRED = ICMP_UGE" << endl;
      result = (uint64_t)v1 >= (uint64_t)v2;
      break;
    case CmpInst::ICMP_ULT:
      if(debug) cout << "PRED = ICMP_ULT" << endl;
      result = (uint64_t)v1 < (uint64_t)v2;
      break;
    case CmpInst::ICMP_ULE:
      if(debug) cout << "PRED = ICMP_ULE" << endl;
      result = (uint64_t)v1 <= (uint64_t)v2;
      break;
    case CmpInst::ICMP_SGT:
      if(debug) cout << "PRED = ICMP_SGT" << endl;
      result = v1 > v2;
      break;
    case CmpInst::ICMP_SGE:
      if(debug) cout << "PRED = ICMP_SGE" << endl;
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_SLT:
      if(debug) cout << "PRED = ICMP_SLT" << endl;
      result = v1 < v2;
      break;
    case CmpInst::ICMP_SLE:
      if(debug) cout << "PRED = ICMP_SLE" << endl;
      result = v1 <= v2;
      break;
    default:
      safe_assert(false);
      break;
  }

  VALUE vresult;
  vresult.as_int = result;

  IValue *nloc = new IValue(INT1_KIND, vresult);
  nloc->setSize(KIND_GetSize(INT1_KIND));

  executionStack.top()[inx] = nloc;
  if (debug) {
    cout << nloc->toString() << endl;
  }
  return;
}

void InterpreterObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  if (debug) {
    printf("<<<<< FCMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(op1).c_str(), KVALUE_ToString(op2).c_str(), pred, inx);
  }

  long double v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else {
    IValue *loc1 = op1->isGlobal ? globalSymbolTable[op1->inx] :
      executionStack.top()[op1->inx];
    v1 = loc1->getFlpValue();
  } 

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else {
    IValue *loc2 = op2->isGlobal ? globalSymbolTable[op2->inx] :
      executionStack.top()[op2->inx];
    v2 = loc2->getFlpValue();
  } 

  if (debug) cout << "v1 = " << v1 << " v2 = " << v2 << endl;

  int result = 0;
  switch(pred) {
    case CmpInst::FCMP_FALSE:
      if (debug)
        cout << "\tCondition is FALSE" << endl;
      result = 0;
      break;
    case CmpInst::FCMP_TRUE:
      if (debug)
        cout << "\tCondition is TRUE" << endl;
      result = 1;
      break;
    case CmpInst::FCMP_UEQ:
      if (debug)
        cout << "\tCondition is UEQ" << endl;
      result = v1 == v2;
      break;
    case CmpInst::FCMP_UNE:
      if (debug)
        cout << "\tCondition is UNE" << endl;
      result = v1 != v2;
      break;
    case CmpInst::FCMP_UGT:
      if (debug)
        cout << "\tCondition is UGT" << endl;
      result = v1 > v2;
      break;
    case CmpInst::FCMP_UGE:
      if (debug)
        cout << "\tCondition is UGE" << endl;
      result = v1 >= v2;
      break;
    case CmpInst::FCMP_ULT:
      if (debug)
        cout << "\tCondition is ULT" << endl;
      result = v1 < v2;
      break;
    case CmpInst::FCMP_ULE:
      if (debug)
        cout << "\tCondition is ULT" << endl;
      result = v1 <= v2;
      break;
    case CmpInst::FCMP_OEQ:
      if (debug)
        cout << "\tCondition is OEQ" << endl;
      result = v1 == v2;
      break;
    case CmpInst::FCMP_ONE:
      if (debug)
        cout << "\tCondition is ONE" << endl; 
      result = v1 != v2;
      break;
    case CmpInst::FCMP_OGT:
      if (debug)
        cout << "\tCondition is OGT" << endl;
      result = v1 > v2;
      break;
    case CmpInst::FCMP_OGE:
      if (debug)
        cout << "\tCondition is OGE" << endl;
      result = v1 >= v2;
      break;
    case CmpInst::FCMP_OLT:
      if (debug)
        cout << "\tCondition is OLT" << endl;
      result = v1 < v2;
      break;
    case CmpInst::FCMP_OLE:
      if (debug)
        cout << "\tCondition is OLE" << endl;
      result = v1 <= v2;
      break;
    default:
      safe_assert(false);
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
  if (debug) {
    printf("<<<<< PHINODE >>>>>: id %s [INX: %d] \n", IID_ToString(iid).c_str(), inx);
  }

  if (debug) {
    cout << recentBlock.top() << endl;
  }

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
  if (debug) {
    cout << phiNode->toString() << endl;
  }

  return;
}

void InterpreterObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {

  if (debug) {
    printf("<<<<< SELECT >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(cond).c_str(), KVALUE_ToString(tvalue).c_str(), 
        KVALUE_ToString(fvalue).c_str(), inx);
  }

  int condition;
  IValue *conditionValue, *trueValue, *falseValue, *result;

  if (cond->inx == -1) {
    condition = cond->value.as_int;
  } else {
    conditionValue = cond->isGlobal ? globalSymbolTable[cond->inx] : executionStack.top()[cond->inx];
    condition = conditionValue->getValue().as_int;
  }


  if (condition) {
    if (tvalue->inx == -1) {
      result = new IValue(tvalue->kind, tvalue->value, REGISTER);
    } else {
      result = new IValue();
      trueValue = tvalue->isGlobal ? globalSymbolTable[tvalue->inx] : executionStack.top()[tvalue->inx];
      trueValue->copy(result);
    }
  } else {
    if (fvalue->inx == -1) {
      result = new IValue(fvalue->kind, fvalue->value, REGISTER);
    } else {
      result = new IValue();
      falseValue = fvalue->isGlobal ? globalSymbolTable[fvalue->inx] : executionStack.top()[fvalue->inx];
      falseValue->copy(result);
    }
  }

  executionStack.top()[inx] = result;

  if (debug) {
    cout << "Result is " << result->toString() << endl;
  }
  return;
}

void InterpreterObserver::push_stack(KVALUE* value) {
  if (debug) {
    printf("<<<<< PUSH ARGS TO STACK >>>>>, value %s\n", KVALUE_ToString(value).c_str());
  }

  myStack.push(value);
}

void InterpreterObserver::push_phinode_constant_value(KVALUE* value, int blockId) {
  if (debug) {
    printf("<<<<< PUSH PHINODE CONSTANT VALUE >>>>> kvalue: %s, blockid: %d\n", KVALUE_ToString(value).c_str(), blockId);
  }
  phinodeConstantValues[blockId] = value;
}

void InterpreterObserver::push_phinode_value(int valId, int blockId) {
  if (debug) {
    printf("<<<<< PUSH PHINODE VALUE >>>>> valId: %d, blockid: %d\n", valId, blockId);
  }
  phinodeValues[blockId] = valId;
}

void InterpreterObserver::push_return_struct(KVALUE* value) {
  if (debug) {
    printf("<<<<< PUSH RETURN STRUCT >>>>> value %s\n", KVALUE_ToString(value).c_str());
  }
  returnStruct.push(value);
}

void InterpreterObserver::push_struct_type(KIND kind) {
  if (debug) {
    printf("<<<<< PUSH STRUCT TYPE >>>>>: %s\n", KIND_ToString(kind).c_str()); 
    cout << structType.size() << endl;
  }
  structType.push(kind);
}

void InterpreterObserver::push_struct_element_size(uint64_t s) {
  if (debug) {
    printf("<<<<< PUSH STRUCT ELEMENT SIZE >>>>> size: %ld\n", s);
  }
  structElementSize.push(s);
}

void InterpreterObserver::push_getelementptr_inx(KVALUE* int_value) {
  int idx = int_value->value.as_int;
  if (debug) {
    printf("<<<<< PUSH GETELEMENTPTR INDEX >>>>>: %s\n", KVALUE_ToString(int_value).c_str());
  }
  getElementPtrIndexList.push(idx);
}

void InterpreterObserver::push_getelementptr_inx2(int int_value) {
  int idx = int_value;
  if (debug) {
    printf("<<<<< PUSH GETELEMENTPTR INDEX >>>>>: %d\n", idx);
  }
  getElementPtrIndexList.push(idx);
}

void InterpreterObserver::push_array_size(uint64_t size) {
  if (debug) {
    printf("<<<<< PUSH ARRAY SIZE >>>>>: %ld\n", size);
  }
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
    reg->setValueOffset(0); // new
    callerVarIndex.pop();

    if (debug) {
      cout << reg->toString() << endl;
    }
  } else {
    if (debug) {
      cout << myStack.size() << endl;
      cout << "callArgs size: " << callArgs.size() << endl;
    }
    safe_assert(callArgs.empty());
    safe_assert(myStack.empty());
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

  safe_assert(!recentBlock.empty());
  recentBlock.pop();

  // empty myStack and callArgs
  while (!myStack.empty()) {
    myStack.pop();
  }
  while (!callArgs.empty()) {
    callArgs.pop();
  }
}

void InterpreterObserver::after_struct_call() {
  if (debug) {
    printf("<<<<< AFTER STRUCT CALL >>>>>");
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

    safe_assert(!returnStruct.empty());

    // reconstruct struct value
    IValue* structValue = (IValue*) malloc(returnStruct.size()*sizeof(IValue));
    unsigned i = 0;
    while (!returnStruct.empty()) {
      KVALUE* value = returnStruct.front();
      IValue* iValue = new IValue(value->kind);
      iValue->setValue(value->value);
      iValue->setLength(0);

      structValue[i] = *iValue; 
      i++;
      returnStruct.pop();
    }

    safe_assert(returnStruct.empty());


    executionStack.top()[callerVarIndex.top()] = structValue;

    if (debug) {
      cout << executionStack.top()[callerVarIndex.top()]->toString() << endl;
    }

    callerVarIndex.pop();
  } else {
    while (!returnStruct.empty()) {
      returnStruct.pop();
    }
    safe_assert(callArgs.empty());
    safe_assert(myStack.empty());
    safe_assert(returnStruct.empty());
  }

  isReturn = false;

  safe_assert(!recentBlock.empty());
  recentBlock.pop();

}

void InterpreterObserver::create_stack_frame(int size) {
  if (debug) {
    printf("<<<<< CREATE STACK FRAME OF SIZE %d >>>>>\n", size);
  }

  isReturn = false;

  std::vector<IValue*> frame (size);
  for (int i = 0; i < size; i++) {
    if (!callArgs.empty()) {
      frame[i] = callArgs.top();
      if (debug) {
        cout << "\t Argument " << i << ": " << frame[i]->toString() << endl;
      }
      callArgs.pop();
    } else {
      frame[i] = new IValue();
    }
  }
  safe_assert(callArgs.empty());
  executionStack.push(frame);
}

void InterpreterObserver::create_global_symbol_table(int size) {
  if (debug) {
    printf("<<<<< CREATE GLOBAL SYMBOL TABLE OF SIZE %d >>>>>\n", size);
  }
  // initialize logger
  google::InitGoogleLogging("main");
  LOG(INFO) << "Initialized logger" << endl;

  for (int i = 0; i < size; i++) {
    IValue* value = new IValue();
    globalSymbolTable.push_back(value);
  }
  return;
}

void InterpreterObserver::record_block_id(int id) {
  if (debug) {
    printf("<<<<< RECORD BLOCK ID >>>>> %d\n", id);
  }

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
  value.as_ptr = kvalue->value.as_ptr;
  IValue* ptrLocation = new IValue(PTR_KIND, value, GLOBAL);
  ptrLocation->setSize(KIND_GetSize(initializer->kind)); // put in constructor
  ptrLocation->setLength(0);
  ptrLocation->setValueOffset((int64_t)location - value.as_int);

  // store it in globalSymbolTable
  globalSymbolTable[kvalue->inx] = ptrLocation;
  if (debug) {
    cout << "\tloc: " << location->toString() << endl;
    cout << "\tptr: " << ptrLocation->toString() << endl;
  }
}

void InterpreterObserver::call(IID iid, bool nounwind, KIND type, int inx) {
  if (debug) {
    printf("<<<<< CALL >>>>> %s, ", IID_ToString(iid).c_str());
    printf(" return type %s,", KIND_ToString(type).c_str());
    printf(" nounwind %d,", (nounwind ? 1 : 0));
    printf(" [INX: %d]\n", inx);
    printf("my stack size: %ld\n", myStack.size());
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
      IValue* arg = value->isGlobal ? globalSymbolTable[value->inx] :
        executionStack.top()[value->inx];
      safe_assert(arg);
      argCopy = new IValue();
      arg->copy(argCopy);
    } else {
      // argument is a constant
      argCopy = new IValue(value->kind, value->value, LOCAL);
      argCopy->setLength(0); // uninitialized pointer
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


void InterpreterObserver::call_malloc(IID iid, bool nounwind, KIND type, KVALUE* call_value, int size, int inx, KVALUE* mallocAddress) {

  if (debug) {
    printf("<<<<< CALL MALLOC >>>>> %s, call_value: %s, return type: %s, nounwind: %d, size:%d, [INX: %d], %s", 
        IID_ToString(iid).c_str(), 
        KVALUE_ToString(call_value).c_str(), 
        KIND_ToString(type).c_str(), 
        (nounwind ? 1 : 0),
        size,
	inx,
        KVALUE_ToString(mallocAddress).c_str());
  }


  // retrieving original number of bytes
  KVALUE* argValue = myStack.top();
  myStack.pop();
  assert(myStack.size() == 0);
  
  if (type != STRUCT_KIND) {
    // allocating space
    int numObjects = argValue->value.as_int*8 / size;
    int actualSize = sizeof(IValue) * numObjects;    
    void *addr = malloc(actualSize);

    // creating pointer object
    VALUE returnValue;
    returnValue.as_ptr = mallocAddress->value.as_ptr;
    IValue* newPointer = new IValue(PTR_KIND, returnValue, size/8, 0, 0, numObjects);
    newPointer->setValueOffset((int64_t)addr - (int64_t)returnValue.as_ptr);
    executionStack.top()[inx] = newPointer;

    // creating locations
    unsigned currOffset = 0;
    for(int i = 0; i < numObjects; i++) {
      // creating object
      VALUE iValue;
      // TODO: check if we need uninitialized value
      IValue *var = new IValue(type, iValue, currOffset);
      ((IValue*)addr)[i] = *var;
      ((IValue*)addr)[i].setValueOffset(newPointer->getValueOffset()); //setting basepointer value offset?

      // updating offset
      currOffset += (size/8);
    }

    if (debug) {
      cout << endl << executionStack.top()[inx]->toString() << endl;
    }
  } else {

    // allocating space
    unsigned numStructs = ceil(argValue->value.as_int*8.0 / size);
    unsigned fields = structType.size();

    int actualSize = sizeof(IValue) * numStructs * fields;    
    void *addr = malloc(actualSize);
    IValue* ptrToStructVar = (IValue*)addr;

    if (debug) {
      cout << "\nTotal size of malloc in bits: " << argValue->value.as_int*8 << endl;
      cout << "Size: " << size << endl;
      cout << "Num Structs: " << numStructs << endl;
      cout << "Number of fields: " << fields << endl;
    }

    KIND fieldTypes[fields];
    for(unsigned i = 0; i < fields; i++) {
      fieldTypes[i] = structType.front();
      structType.pop();
    }

    unsigned length = 0;
    unsigned firstByte = 0;
    for(unsigned i = 0; i < numStructs; i++) {
      for (unsigned j = 0; j < fields; j++) {
        KIND type = fieldTypes[j];
        IValue* var = new IValue(type);
        var->setFirstByte(firstByte);
        firstByte = firstByte + KIND_GetSize(type);
        ptrToStructVar[length] = *var;
        if (debug) {
          cout << "Created a field of struct: " << length << endl;
          cout << ptrToStructVar[length].toString() << endl;
        }
        length++;
      }
    }

    safe_assert(structType.empty());

    VALUE structPtrVal;
    structPtrVal.as_ptr = mallocAddress->value.as_ptr;
    IValue* structPtrVar = new IValue(PTR_KIND, structPtrVal);
    structPtrVar->setValueOffset((int64_t)ptrToStructVar - (int64_t)mallocAddress->value.as_ptr);  ////////////
    structPtrVar->setSize(KIND_GetSize(ptrToStructVar[0].getType()));
    structPtrVar->setLength(length);

    executionStack.top()[inx] = structPtrVar;
    if (debug) {
      cout << structPtrVar->toString() << endl;
    }
  }

  return;
}

void InterpreterObserver::vaarg() {
  if (debug) {
    printf("<<<<< VAARG >>>>>\n");
  }

  cerr << "[InterpreterObserver::vaarg] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::landingpad() {
  if (debug) {
    printf("<<<<< LANDINGPAD >>>>>\n");
  }

  cerr << "[InterpreterObserver::landingpad] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::printCurrentFrame() {
  if (debug) {
    printf("Print current frame\n");
  }
}

/**
 *
 * @param iValue the interpreted iValue of the concrete value
 * @param concrete pointer to the concrete value
 */
bool InterpreterObserver::syncLoad(IValue* iValue, KVALUE* concrete, KIND type) { 
  bool sync = false;
  VALUE syncValue;
  int64_t cValueVoid;
  int8_t* cValueInt8Arr;
  int16_t cValueInt16;
  int16_t* cValueInt16Arr;
  int32_t cValueInt32;
  int32_t* cValueInt32Arr;
  int64_t cValueInt64;
  float cValueFloat;
  double cValueDouble;
  long double cValueLD;

  switch (type) {
    case PTR_KIND:
      // TODO: we use int64_t to represent a void* here
      // might not work on 32 bit machine
      cValueVoid = *((int64_t*) concrete->value.as_ptr);

      if (debug) {
        cout << "index: " << iValue->getIndex() << endl;
        cout << "size: " << iValue->getSize() << endl;
        cout << "value: " << iValue->getValue().as_int << endl;
        cout << "first value: " << iValue->getValue().as_int + iValue->getOffset() << endl;
        cout << "second value: " << cValueVoid << endl;
      }

      sync = (iValue->getValue().as_int + iValue->getOffset() != cValueVoid);
      if (sync) {
        syncValue.as_int = cValueVoid;
        iValue->setValue(syncValue);
      }
      break;
    case INT1_KIND: 
    case INT8_KIND: 
      cValueInt32 = *((int32_t*) concrete->value.as_ptr);
      cValueInt32 = cValueInt32 & 0x000000FF;

      sync = (((int8_t) iValue->getIntValue()) != cValueInt32);
      if (sync) {
        if (debug) {
          cout << "\tINT8_KIND case: " << endl;
          cout << "\t IVALUE: " << iValue->getValue().as_int << endl; 
          cout << "\t CONCRETE: " << cValueInt32 << endl; 
          cout << "\t CONCRETE FULL: " << concrete->value.as_int << endl;
        }
        cValueInt8Arr = (int8_t*) calloc(8, sizeof(int8_t));
        cValueInt8Arr[0] = cValueInt32;
        syncValue.as_int = *((int64_t*) cValueInt8Arr);
        iValue->setValue(syncValue);
        if (debug) {
          cout << "\t AFTER SYNC IVALUE: " << iValue->getValue().as_int << endl;
        }
      }
      break;
    case INT16_KIND: 
      cValueInt16 = *((int16_t*) concrete->value.as_ptr);
      sync = (((int16_t) iValue->getIntValue()) != cValueInt16);
      if (sync) {
        cValueInt16Arr = (int16_t*) calloc(4, sizeof(int16_t)); 
        cValueInt16Arr[0] = cValueInt16; 
        syncValue.as_int = *((int64_t*) cValueInt16Arr);
        iValue->setValue(syncValue);
      }
      break;
    case INT24_KIND:
      cValueInt32 = *((int32_t*) concrete->value.as_ptr);
      cValueInt32 = cValueInt32 & 0x00FFFFFF;
      sync = (((int32_t) iValue->getIntValue()) != cValueInt32);
      if (sync) {
        if (debug) {
          cout << "\t INT24_KIND case: " << endl;
          cout << "\t IVALUE: " << iValue->getValue().as_int << endl; 
          cout << "\t CONCRETE: " << cValueInt32 << endl; 
          cout << "\t CONCRETE 64: " << KVALUE_ToIntValue(concrete) << endl;
          cout << "\t CONCRETE FULL: " << concrete->value.as_int << endl;
	  cout << "\t CONCRETE IN HEX: " << concrete->value.as_ptr << endl;
        }
        cValueInt32Arr = (int32_t*) calloc(2, sizeof(int32_t));
        cValueInt32Arr[0] = cValueInt32; 
        syncValue.as_int = *((int32_t*) cValueInt32Arr);
        iValue->setValue(syncValue);
      }
      break;
    case INT32_KIND: 
      if (debug) cout << "\t SYNCING INT32_KIND " << endl;
      if (debug) cout << iValue->toString() << endl;
      if (debug) cout << KVALUE_ToString(concrete) << endl;
      cValueInt32 = *((int32_t*) concrete->value.as_ptr);
      sync = (((int32_t) iValue->getIntValue()) != cValueInt32);
      if (sync) {
        if (debug) {
          cout << "\t IVALUE: " << iValue->getValue().as_int << endl; 
        }
        cValueInt32Arr = (int32_t*) calloc(2, sizeof(int32_t));
        cValueInt32Arr[0] = cValueInt32; 
        syncValue.as_int = *((int32_t*) cValueInt32Arr);
        iValue->setValue(syncValue);
      }
      break;
    case INT64_KIND:
      cValueInt64 = *((int64_t*) concrete->value.as_ptr);
      sync = (iValue->getValue().as_int != cValueInt64);
      if (sync) {
        syncValue.as_int = cValueInt64;
        iValue->setValue(syncValue);
      }
      break;
    case INT80_KIND:
      cout << "[syncload] Unsupported INT80_KIND" << endl;
      safe_assert(false);
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
      LOG(INFO) << "SYNCING AT LOAD DUE TO MISMATCH" << endl;
      cout << "\t SYNCING AT LOAD DUE TO MISMATCH" << endl;
      cout << "\t " << iValue->toString() << endl;
    }
  }

  return sync;
}
