/**
 * @file InterpreterObserver.cpp
 * @brief
 */

/*
 * Copyright (c) 2013, UC Berkeley All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1.  Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software must
 * display the following acknowledgement: This product includes software
 * developed by the UC Berkeley.
 *
 * 4. Neither the name of the UC Berkeley nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY UC BERKELEY ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL UC BERKELEY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen and Cindy Rubio-Gonzalez

#include "InterpreterObserver.h"

#include "stdbool.h"
#include <assert.h>
#include <math.h>
#include <stack>
#include <vector>

#include <llvm/IR/InstrTypes.h>
#include <glog/logging.h>

/***************************** Helper Functions *****************************/


void release(IValue* value) {
  if (value->getScope() == REGISTER || value->getScope() == LOCAL) {
    if (!value->isStruct()) {
      delete value;
    }
    else {
      //cout << "DID not delete because it is a struct" << endl;
    }
  }
  else {
    cout << "[GLOBAL?] " << value->toString() << endl;
  }
  return;
}



/*
void release(IValue* value) {
  if (value->getType() == PTR_KIND) {
    IValue *other = (IValue*)value->getIPtrValue();
    //cout << "[RELEASE] " << other->toString() << endl; 
    release(other);
  }
  delete value;
  return;
}
*/

unsigned InterpreterObserver::findIndex(IValue* array, unsigned offset, unsigned length) {
  int low, high, index;

  // assert: offset cannot larger than the size of the array itself (int byte)
  // safe_assert(offset <= array[length-1].getFirstByte() + KIND_GetSize(array[length-1].getType()));

  // initializing lowerbound and upperbound of the index
  low = 0;
  high = length - 1;

  DEBUG_STDOUT("\t" << "[findIndex] Offset: " << offset << " Length: " << length);

  // search for the index using binary search
  // the IValue at the index should have the largest byteOffset that is less
  // than or equal to the offset
  index = -1;
  while(low < high){

    unsigned mid, firstByte;
    mid = (low + high) / 2;

    DEBUG_STDOUT("\t" << "[findIndex] Mid index: " << mid);

    firstByte = array[mid].getFirstByte();

    DEBUG_STDOUT("\t" << "[findIndex] Firstbyte: " << firstByte);

    if (firstByte == offset) {
      index = mid;
      break;
    } 
    else if ((mid + 1 <= length -1) && (firstByte < offset) && (offset < array[mid+1].getFirstByte())) {
      index = mid;
      break;
    } 
    else if (offset < firstByte) {
      high = mid - 1;
    } 
    else {
      low = mid + 1;
    }
  }

  index = (index == -1) ? high : index;
  DEBUG_STDOUT("\t" << "[findIndex] Returning index: " << index); 
  return index; 
}

bool InterpreterObserver::checkStore(IValue *dest, KIND srcKind, int64_t srcValue) {
  bool result; 
  double dpValue;
  double *dpPtr;
  int32_t srcValue32;

  result = false;
  dpPtr = (double *) &srcValue; 
  dpValue = *dpPtr;

  switch(srcKind) {
    case PTR_KIND:

      DEBUG_STDOUT("\t Destination value: " << (int64_t) dest->getValue().as_ptr);
      DEBUG_STDOUT("\t Destination value plus offset: " << (int64_t)
          dest->getValue().as_ptr + dest->getOffset());
      DEBUG_STDOUT("\t Concrete value: " << (int64_t) srcValue);

      result = ((int64_t)dest->getValue().as_ptr + dest->getOffset() == srcValue);
      break;
    case INT1_KIND:
      result = ((bool)dest->getValue().as_int == (bool)srcValue);
      break;
    case INT8_KIND: 
      result = ((int8_t)dest->getValue().as_int == (int8_t)srcValue);
      break;
    case INT16_KIND: 
      result = ((int16_t)dest->getValue().as_int == (int16_t)srcValue);
      break;
    case INT24_KIND:
      srcValue32 = srcValue;
      srcValue32 = srcValue32 & 0x00FFFFFF;
      result = (dest->getIntValue() == srcValue32);
      break;
    case INT32_KIND: 
      result = ((int32_t)dest->getValue().as_int == (int32_t)srcValue);
      break;
    case INT64_KIND:
      result = (dest->getValue().as_int == srcValue);
      break;
    case FLP32_KIND:
      if (isnan((float)dest->getValue().as_flp) && isnan((float)dpValue)) {
        result = true;
      } 
      else {
        result = ((float)dest->getValue().as_flp) == ((float)dpValue);
      }
      break;
    case FLP64_KIND:
      if (isnan((double)dest->getValue().as_flp) && isnan((double)dpValue)) {
        result = true;
      }
      else {
        result = ((double)dest->getValue().as_flp) == ((double)dpValue);
      }
      break;
    case FLP80X86_KIND:
      result = dest->getValue().as_flp == dpValue;
      break;
    default: //safe_assert(false);
      break;
  }
  return result;
}

bool InterpreterObserver::checkStore(IValue *dest, KVALUE *kv) {
  bool result = false;

  switch(kv->kind) {
    case PTR_KIND:

      DEBUG_STDOUT("\t Destination value: " << (int64_t) dest->getValue().as_ptr);
      DEBUG_STDOUT("\t Destination value plus offset: " << (int64_t)
          dest->getValue().as_ptr + dest->getOffset());
      DEBUG_STDOUT("\t Concrete value: " << (int64_t) kv->value.as_ptr);

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
      result = (dest->getValue().as_int == kv->value.as_int);
      break;
    case FLP32_KIND:
      if (isnan((float)dest->getValue().as_flp) && isnan((float)kv->value.as_flp)) {
        result = true;
      }
      else {
        result = ((float)dest->getValue().as_flp) == ((float)kv->value.as_flp);
      }
      break;
    case FLP64_KIND:
      if (isnan((double)dest->getValue().as_flp) && isnan((double)kv->value.as_flp)) {
        result = true;
      }
      else {
        result = ((double)dest->getValue().as_flp) == ((double)kv->value.as_flp);
        if (!result) {
          DEBUG_STDOUT(dest->getValue().as_ptr);
          DEBUG_STDOUT(kv->value.as_ptr);
        }
      }
      break;
    case FLP80X86_KIND:
      result = dest->getValue().as_flp == kv->value.as_flp;
      break;
    default: //safe_assert(false);
      break;
  }
  return result;
}

void InterpreterObserver::copyShadow(IValue* src UNUSED, IValue* dest UNUSED) {
  // do nothing
  // shadow value is not used in the core interpreter
}

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
      DEBUG_STDERR("Unsupport binary operator operand: " << binop);
      safe_assert(false);
      break;
  }
  return s.str();
}

std::string InterpreterObserver::BITWISE_ToString(int bitwise) {
  std::stringstream s;
  switch(bitwise) {
    case SHL:
      s << "SHL";
      break;
    case LSHR:
      s << "LSHR";
      break;
    case ASHR:
      s << "ASHR";
      break;
    case AND:
      s << "AND";
      break;
    case OR:
      s << "OR";
      break;
    case XOR:
      s << "XOR";
      break;
    default: 
      DEBUG_STDERR("Unsupport bitwise operator operand: " << bitwise);
      safe_assert(false);
      break;
  }
  return s.str();
}

std::string InterpreterObserver::CASTOP_ToString(int castop) {
  std::stringstream s;
  switch(castop) {
    case TRUNC:
      s << "TRUNC";
      break;
    case ZEXT:
      s << "ZEXT";
      break;
    case SEXT:
      s << "SEXT";
      break;
    case FPTRUNC:
      s << "FPTRUNC";
      break;
    case FPEXT:
      s << "FPEXT";
      break;
    case FPTOUI:
      s << "FPTOUI";
      break;
    case FPTOSI:
      s << "FPTOSI";
      break;
    case UITOFP:
      s << "UITOFP";
      break;
    case SITOFP:
      s << "SITOFP";
      break;
    case PTRTOINT:
      s << "PTRTOINT";
      break;
    case INTTOPTR:
      s << "INTTOPTR";
      break;
    case BITCAST:
      s << "BITCAST";
      break;
    default: 
      DEBUG_STDERR("Unsupport bitwise operator operand: " << castop);
      safe_assert(false);
      break;
  }
  return s.str();
}

/***************************** Interpretation *****************************/

// *** Load and Store Operations *** //

void InterpreterObserver::load_struct(IID iid UNUSED, KIND type UNUSED, KVALUE* src, int inx) {
  unsigned structSize;
  IValue* dest;

  //DEBUG_LOG("[LOAD STRUCT] Performing load "); 

  structSize = returnStruct.size();
  dest = new IValue[structSize];
    
  if (src->inx == -1) {

    // Case 1: struct constant.
    // Create an IValue struct that has all values in structReturn.
    for(unsigned i = 0; structSize; i++) {
      KVALUE* concreteStructElem; 
      IValue* structElem; 
      
      concreteStructElem = returnStruct[i];

      if (concreteStructElem->inx == -1) {
        structElem = new IValue(concreteStructElem->kind, concreteStructElem->value, REGISTER);
      } 
      else {
        structElem = concreteStructElem->isGlobal ?
          globalSymbolTable[concreteStructElem->inx] :
          executionStack.top()[concreteStructElem->inx];
      }
      dest[i] = *structElem;
    }
    returnStruct.clear();
    
    safe_assert(false); // why?

  } 
  else {
    
    // Case 2: local or global struct.
    
    IValue* srcPointer;
    IValue* structSrc;

    srcPointer = src->isGlobal ? globalSymbolTable[src->inx] : executionStack.top()[src->inx];
    structSrc = (IValue*) srcPointer->getIPtrValue();

    for(unsigned i = 0; i < structSize; i++) {
      KVALUE *concreteStructElem, *concreteStructElemPtr; 
      IValue *structElem;
      int type;

      // get concrete value in case we need to sync
      concreteStructElem = returnStruct[i];

      structElem = new IValue();
      structSrc[i].copy(structElem);
      type = structElem->getType();

      // sync load
      // first create a KVALUE pointer to concreteStructElem because sync load
      // expect the KVALUE to be a pointer to the concrete value
      concreteStructElemPtr = new KVALUE();
      concreteStructElemPtr->value.as_ptr = &(concreteStructElem->value);
      if (syncLoad(structElem, concreteStructElemPtr, type)) {
        DEBUG_LOG("[LOAD STRUCT] Syncing load");
      }
      dest[i] = *structElem;
    }
    returnStruct.clear();
  }

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = dest;

  DEBUG_STDOUT("Destination result: " << dest->toString());
  return;
}

void InterpreterObserver::load(IID iid UNUSED, KIND type, SCOPE opScope, int opInx, uint64_t opAddr, bool loadGlobal, 
			       int loadInx, int inx) {

  //pre_load(iid, type, opScope, opInx, opAddr, loadGlobal, loadInx, file, line, inx);

  bool isPointerConstant = false;
  bool sync = false;
  IValue* srcPtrLocation;
  IValue* destLocation = executionStack.top()[inx];

  //DEBUG_LOG("[LOAD] Performing load");

  // retrieving source pointer value
  if (opScope == CONSTANT) {
    isPointerConstant = true;
  } 
  else if (opScope == GLOBAL) {
    srcPtrLocation = globalSymbolTable[opInx];
  } 
  else {
    srcPtrLocation = executionStack.top()[opInx];
  }

  // performing load
  if (!isPointerConstant) {

    DEBUG_STDOUT("\tsrcPtrLocation: " << srcPtrLocation->toString());

    // CASE 1: srcPtrLocation and srcLocation exist
    if (srcPtrLocation->isInitialized()) {

      // retrieving source
      IValue *values = (IValue*)srcPtrLocation->getIPtrValue();
      unsigned valueIndex = srcPtrLocation->getIndex();
      IValue* srcLocation = values + valueIndex;

      // calculating internal offset
      unsigned srcOffset = srcPtrLocation->getOffset();
      unsigned currOffset = values[valueIndex].getFirstByte();
      int internalOffset = srcOffset - currOffset;

      // retrieving value given the internal offset
      VALUE value = srcPtrLocation->readValue(internalOffset, type);

      DEBUG_STDOUT("\t\tvalueIndex: " << valueIndex);
      DEBUG_STDOUT("\t\tsrcOffset: " << srcOffset);
      DEBUG_STDOUT("\t\tcurrOffset: " << currOffset);
      DEBUG_STDOUT("\t\tsrcOffset" << srcOffset);
      DEBUG_STDOUT("\t\tinternal offset: " << internalOffset);
      DEBUG_STDOUT("\tsrcLocation: " << srcLocation->toString());
      DEBUG_STDOUT("\tCalling readValue with internal offset: " << internalOffset << " and size: " << KIND_GetSize(type)); 
      DEBUG_STDOUT("\t\tVALUE returned (float): " << value.as_flp);
      DEBUG_STDOUT("\t\tVALUE returned (int): " << value.as_int);

      // copying src into dest
      srcLocation->copy(destLocation);
      destLocation->setTypeValue(type, value);

      // syncing load value with concrete value
      sync = syncLoad(destLocation, opAddr, type);

      // if sync happens, update srcPtrLocation if possible
      if (sync) {
        IValue *lastElement = values + srcPtrLocation->getLength() - 1;

        if (srcOffset + KIND_GetSize(type) <= lastElement->getFirstByte() + KIND_GetSize(lastElement->getType())) {
          srcPtrLocation->writeValue(internalOffset, KIND_GetSize(type), destLocation);
        }
      }
    }
    else {
      // CASE 2: srcPtrLocation exists, but it is not initialized (no srcLocation)
      DEBUG_STDOUT("\tSource pointer is not initialized!");

      VALUE zeroValue;
      IValue* srcLocation;
      zeroValue.as_int = 0;

      destLocation->clear();
      destLocation->setTypeValue(type, zeroValue);

      // syncing load
      sync = syncLoad(destLocation, opAddr, type);

      // initializing srcPtrLocation and srcLocation
      DEBUG_STDOUT("\tInitializing source pointer.");
      DEBUG_STDOUT("\tSource pointer location: " << srcPtrLocation->toString());
      srcLocation = new IValue();
      destLocation->copy(srcLocation);
      srcPtrLocation->setLength(1); // initialized
      srcPtrLocation->setSize(KIND_GetSize(type));
      srcPtrLocation->setValueOffset((int64_t)srcLocation - srcPtrLocation->getValue().as_int);
      DEBUG_STDOUT("\tSource pointer location: " << srcPtrLocation->toString());

      // TODO: revise this case
      // updating load variable
      if (loadInx != -1) {
        IValue *elem, *values, *loadInst;
        loadInst = loadGlobal ? globalSymbolTable[loadInx] : executionStack.top()[loadInx];

        // retrieving source
        values = (IValue*)loadInst->getIPtrValue();
        elem = values + loadInst->getIndex();
        elem->setLength(srcPtrLocation->getLength());
        elem->setSize(srcPtrLocation->getSize());
        elem->setValueOffset(srcPtrLocation->getValueOffset());
      }
    }
    DEBUG_STDOUT(destLocation->toString());
  }
  else {
    // CASE 3: Neither srcPtrLocation nor srcLocation exist
    // TODO: revise again
    DEBUG_STDOUT("[Load] => pointer constant.");

    VALUE zeroValue;
    zeroValue.as_int = 0;

    destLocation->clear();
    destLocation->setTypeValue(type, zeroValue);

    // syncing load
    sync = syncLoad(destLocation, opAddr, type);
    DEBUG_STDOUT(destLocation->toString());
  }

  if (sync) {
    DEBUG_LOG("[LOAD] Syncing load");
  }

  //post_load(iid, type, opScope, opInx, opAddr, loadGlobal, loadInx, file, line, inx);
  return;
}

void InterpreterObserver::store(int dstInx, SCOPE dstScope, KIND srcKind, SCOPE srcScope, int srcInx, int64_t srcValue) {

  //pre_store(destInx, destScope, srcKind, srcScope, srcInx, srcValue, file, line, inx);

  if (srcKind == INT80_KIND) {
    cout << "[store] Unsupported INT80_KIND" << endl;
    safe_assert(false);
    return; // otherwise compiler warning
  }

  // pointer constant; we simply ignore this case
  if (dstScope == CONSTANT) {
    DEBUG_STDOUT("\tIgnoring pointer constant.");
    return; 
  }

  // retrieving destination pointer operand
  IValue* dstPtrLocation = (dstScope == GLOBAL) ? globalSymbolTable[dstInx] :
    executionStack.top()[dstInx];

  DEBUG_STDOUT("\tDstPtr: " << dstPtrLocation->toString());

  // the destination pointer is not initialized
  // initialize with an empty IValue object
  if (!dstPtrLocation->isInitialized()) {
    DEBUG_STDOUT("\tDestination pointer location is not initialized");
    IValue* iValue = new IValue(srcKind);
    iValue->setLength(0);
    dstPtrLocation->setValueOffset((int64_t)iValue - (int64_t)dstPtrLocation->getPtrValue()); 
    dstPtrLocation->setInitialized();
    DEBUG_STDOUT("\tInitialized destPtr: " << dstPtrLocation->toString());
  }

  unsigned dstPtrOffset = dstPtrLocation->getOffset();
  IValue *dstLocation = NULL;
  IValue *srcLocation = NULL;
  int internalOffset = 0;

  // retrieving source
  bool removeSrc = false;
  if (srcScope == CONSTANT) {
    VALUE value;
    value.as_int = srcValue;

    srcLocation = new IValue(srcKind, value);
    removeSrc = true;
    srcLocation->setLength(0); // uninitialized constant pointer 
    if (srcKind == INT1_KIND) {
      srcLocation->setBitOffset(1);
    }
  } 
  else if (srcScope == GLOBAL) {
    srcLocation = globalSymbolTable[srcInx];
  } 
  else {
    srcLocation = executionStack.top()[srcInx];
  }

  DEBUG_STDOUT("\tSrc: " << srcLocation->toString());

  // retrieve actual destination
  IValue* values = (IValue*)dstPtrLocation->getIPtrValue();
  unsigned valueIndex = dstPtrLocation->getIndex();
  unsigned currOffset = values[valueIndex].getFirstByte();
  dstLocation = values + valueIndex;
  internalOffset = dstPtrOffset - currOffset;

  DEBUG_STDOUT("\tdstPtrOffset: " << dstPtrOffset);
  DEBUG_STDOUT("\tvalueIndex: " << valueIndex << " currOffset: " << currOffset << " Other offset: "  << dstPtrOffset);
  DEBUG_STDOUT("\tinternalOffset: " << internalOffset <<  " Size: " << dstPtrLocation->getSize());
  DEBUG_STDOUT("\tDst: " << dstLocation->toString());
  DEBUG_STDOUT("\tCalling writeValue with offset: " << internalOffset << ", size: " << dstPtrLocation->getSize());

  // writing src into destination
  if (dstPtrLocation->writeValue(internalOffset, KIND_GetSize(srcKind), srcLocation)) {
    srcLocation->copy(dstLocation);
  }
  dstPtrLocation->setInitialized();

  DEBUG_STDOUT("\tUpdated Dst: " << dstLocation->toString());
  DEBUG_STDOUT("\tCalling readValue with internal offset: " << internalOffset << " size: " << dstPtrLocation->getSize());

  // sanity check
  IValue* writtenValue = new IValue(srcLocation->getType(),
      dstPtrLocation->readValue(internalOffset, srcKind)); 
  writtenValue->setSize(dstLocation->getSize());
  writtenValue->setIndex(dstLocation->getIndex());
  writtenValue->setOffset(dstLocation->getOffset());
  writtenValue->setBitOffset(dstLocation->getBitOffset());

  DEBUG_STDOUT("\twrittenValue: " << writtenValue->toString());

  if (!checkStore(writtenValue, srcKind, srcValue)) { // destLocation
    double *ptr = (double*) &srcValue;
    DEBUG_STDERR("\twrittenValue: " << writtenValue->toString());
    DEBUG_STDERR("\tconcreteType: " << KIND_ToString(srcKind));
    DEBUG_STDERR("\tconcreteValue in int64: " << srcValue);
    DEBUG_STDERR("\tconcreteValue in double: " << *ptr);
    DEBUG_STDERR("\tMismatched values found in Store");
    safe_assert(false);
  }
  delete writtenValue;
  if (removeSrc) { 
    delete srcLocation; 
  }

  DEBUG_STDOUT("\tsrcLocation: " << srcLocation->toString());
  //post_store(destInx, destScope, srcKind, srcScope, srcInx, srcValue, file, line, inx);
  return;
}

// **** Binary Operations *** //
inline void InterpreterObserver::binop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, 
				int inx, BINOP op) {

  IValue* iResult = executionStack.top()[inx];
  iResult->clear();

  if (type == INT80_KIND) {
    DEBUG_STDERR("Unsupported INT80_KIND");
    safe_assert(false);
    return; 
  }

  int64_t v1, v2;
  double d1, d2;
  VALUE result;

  // Get values from two operands. They can be either integer or double so we
  // need 4 variables.
  if (lScope == CONSTANT) { // constant
    double *ptr = (double *) &lValue;
    v1 = lValue;
    d1 = *ptr;
  } 
  else { // register
    IValue *loc1 = (lScope == GLOBAL) ? globalSymbolTable[lValue] : executionStack.top()[lValue];
    v1 = loc1->getIntValue();
    d1 = loc1->getFlpValue();
    DEBUG_STDOUT("\tOperand 01: " << loc1->toString());
  }

  if (rScope == CONSTANT) { // constant
    double *ptr = (double *) &rValue;
    v2 = rValue;
    d2 = *ptr;
  } 
  else { // register
    IValue *loc2 = (rScope == GLOBAL) ? globalSymbolTable[rValue] : executionStack.top()[rValue];
    v2 = loc2->getIntValue();
    d2 = loc2->getFlpValue();
    DEBUG_STDOUT("\tOperand 02: " << loc2->toString());
  }

  DEBUG_STDOUT("\tv1: " << v1 << " v2: " << v2);
  DEBUG_STDOUT("\td1: " << d1 << " d2: " << d2);

  switch (op) {
    case ADD:
      result.as_int = v1 + v2;
      break;
    case SUB:
      result.as_int = v1 - v2;
      break;
    case MUL:
      result.as_int = v1 * v2;
      break;
    case SDIV:
      result.as_int = v1 / v2;
      break;
    case SREM:
      result.as_int = v1 % v2;
      break;
    case UDIV:
      result.as_int = (uint64_t) v1 / (uint64_t) v2;
      break;
    case UREM:
      result.as_int = (uint64_t) v1 % (uint64_t) v2;
      break;
    case FADD:
      result.as_flp = d1 + d2;
      break;
    case FSUB:
      result.as_flp = d1 - d2;
      break;
    case FMUL:
      result.as_flp = d1 * d2;
      break;
    case FDIV:
      result.as_flp = d1 / d2;
      break;
    default:
      DEBUG_STDERR("Unsupported binary operator: " << BINOP_ToString(op)); 
      safe_assert(false);
  }
  iResult->setTypeValue(type, result);  
  DEBUG_STDOUT(iResult->toString());
  return;
}

void InterpreterObserver::add(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  binop(lScope, rScope, lValue, rValue, type, inx, ADD);
}

void InterpreterObserver::fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  //pre_fadd(lScope, rScope, lValue, rValue, type, file, line, col, inx);
  binop(lScope, rScope, lValue, rValue, type, inx, FADD);
  //post_fadd(lScope, rScope, lValue, rValue, type, file, line, col, inx);
}

void InterpreterObserver::sub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  binop(lScope, rScope, lValue, rValue, type, inx, SUB);
}

void InterpreterObserver::fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  //pre_fsub(lScope, rScope, lValue, rValue, type, file, line, col, inx);
  binop(lScope, rScope, lValue, rValue, type, inx, FSUB);
  //post_fsub(lScope, rScope, lValue, rValue, type, file, line, col, inx);
}

void InterpreterObserver::mul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  binop(lScope, rScope, lValue, rValue, type, inx, MUL);
}

void InterpreterObserver::fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  //pre_fmul(lScope, rScope, lValue, rValue, type, file, line, col, inx);
  binop(lScope, rScope, lValue, rValue, type, inx, FMUL);
  //post_fmul(lScope, rScope, lValue, rValue, type, file, line, col, inx);
}

void InterpreterObserver::udiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  binop(lScope, rScope, lValue, rValue, type, inx, UDIV);
}

void InterpreterObserver::sdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  binop(lScope, rScope, lValue, rValue, type, inx, SDIV);
}

void InterpreterObserver::fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  //pre_fdiv(lScope, rScope, lValue, rValue, type, file, line, col, inx);
  binop(lScope, rScope, lValue, rValue, type, inx, FDIV);
  //post_fdiv(lScope, rScope, lValue, rValue, type, file, line, col, inx);
}

void InterpreterObserver::urem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  binop(lScope, rScope, lValue, rValue, type, inx, UREM);
}

void InterpreterObserver::srem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  binop(lScope, rScope, lValue, rValue, type, inx, SREM);
}

void InterpreterObserver::frem(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, 
			       int inx UNUSED) {
  DEBUG_STDERR("UNSUPPORTED IN C???");
  safe_assert(false);
}

// **** Bitwise Operations *** //

void InterpreterObserver::bitwise(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx, BITWISE op) {
  int64_t v64_1, v64_2;
  uint64_t uv64_1, uv64_2;
  int32_t v32_1, v32_2;
  uint32_t uv32_1, uv32_2;
  int16_t v16_1, v16_2; 
  uint16_t uv16_1, uv16_2;
  int8_t v8_1, v8_2;
  uint8_t uv8_1, uv8_2; 

  VALUE result;
  IValue* iResult;

  // INT80_KIND is not supported right now
  if (type == INT80_KIND) {
    DEBUG_STDERR("Unsupported INT80.");
    safe_assert(false);
    return;
  }

  // Get values of two operands
  if (lScope == CONSTANT) {
    v64_1 = lValue;
  } 
  else {
    IValue* iOp1 = (lScope == GLOBAL) ? globalSymbolTable[lValue] : executionStack.top()[lValue];
    v64_1 = iOp1->getIntValue();
  }

  if (rScope == CONSTANT) {
    v64_2 = rValue;
  } 
  else {
    IValue* iOp2 = (rScope == GLOBAL) ? globalSymbolTable[rValue] : executionStack.top()[rValue];
    v64_2 = iOp2->getIntValue();
  }

  // Initialize values for other integer variables depending on type
  v8_1 = (int8_t) v64_1;
  uv8_1 = (uint8_t) v8_1;
  v8_2 = (int8_t) v64_2;
  uv8_2 = (uint8_t) v8_2;
  v16_1 = (int16_t) v64_1;
  uv16_1 = (uint16_t) v16_1;
  v16_2 = (int16_t) v64_2;
  uv16_2 = (uint16_t) v16_2;
  v32_1 = (int32_t) v64_1;
  uv32_1 = (uint32_t) v32_1;
  v32_2 = (int32_t) v64_2;
  uv32_2 = (uint32_t) v32_2;
  uv64_1 = (uint64_t) v64_1;
  uv64_2 = (uint64_t) v64_2;

  // Compute the result of the bitwise operator
  switch (op) {
    case SHL:
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
          result.as_int = uv8_1 << uv8_2;
          break;
        case INT16_KIND:
          result.as_int = uv16_1 << uv16_2;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = uv32_1 << uv32_2;
          break;
        case INT64_KIND:
          result.as_int = uv64_1 << uv64_2;
          break;
        default:
          DEBUG_STDERR("Unsupport integer type: " << type);
          safe_assert(false);
          return;
      }
      break;

    case LSHR:
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
          result.as_int = uv8_1 >> uv8_2;
          break;
        case INT16_KIND:
          result.as_int = uv16_1 >> uv16_2;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = uv32_1 >> uv32_2;
          break;
        case INT64_KIND:
          result.as_int = uv64_1 >> uv64_2;
          break;
        default:
          DEBUG_STDERR("Unsupport integer type: " << type);
          safe_assert(false);
          return;
      }
      break;

    case ASHR:
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
          result.as_int = uv8_1 >> uv8_2;
          break;
        case INT16_KIND:
          result.as_int = uv16_1 >> uv16_2;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = uv32_1 >> uv32_2;
          break;
        case INT64_KIND:
          result.as_int = uv64_1 >> uv64_2;
          break;
        default:
          DEBUG_STDERR("Unsupport integer type: " << type);
          safe_assert(false);
          return;
      }
      break;

    case AND:
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
          result.as_int = v8_1 & v8_2;
          break;
        case INT16_KIND:
          result.as_int = v16_1 & v16_2;
          break;
        case INT24_KIND:
          result.as_int = v32_1 & v32_2;
          result.as_int = result.as_int & 0x00FFFFFF;
        case INT32_KIND:
          result.as_int = v32_1 & v32_2;
          break;
        case INT64_KIND:
          result.as_int = v64_1 & v64_2;
          break;
        default:
          DEBUG_STDERR("Unsupport integer type: " << type);
          safe_assert(false);
          return;
      }
      break;

    case OR:
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
          result.as_int = v8_1 | v8_2;
          break;
        case INT16_KIND:
          result.as_int = v16_1 | v16_2;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = v32_1 | v32_2;
          break;
        case INT64_KIND:
          result.as_int = v64_1 | v64_2;
          break;
        default:
          DEBUG_STDERR("Unsupport integer type: " << type);
          safe_assert(false);
          return;
      }
      break;

    case XOR:
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
          result.as_int = v8_1 ^ v8_2;
          break;
        case INT16_KIND:
          result.as_int = v16_1 ^ v16_2;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = v32_1 ^ v32_2;
          break;
        case INT64_KIND:
          result.as_int = v64_1 ^ v64_2;
          break;
        default:
          DEBUG_STDERR("Unsupported integer type: " << type);
          safe_assert(false);
          return;
      }
      break;


    default:
      DEBUG_STDERR("Unsupported bitwise operator: " << BITWISE_ToString(op));
      safe_assert(false);
      return;
  }

  iResult = executionStack.top()[inx];
  iResult->clear();
  iResult->setTypeValue(type, result);
  DEBUG_STDOUT(iResult->toString());
  return;
} 

void InterpreterObserver::shl(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  bitwise(lScope, rScope, lValue, rValue, type, inx, SHL);
}

void InterpreterObserver::lshr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  bitwise(lScope, rScope, lValue, rValue, type, inx, LSHR);
}

void InterpreterObserver::ashr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  bitwise(lScope, rScope, lValue, rValue, type, inx, ASHR);
}

void InterpreterObserver::and_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  bitwise(lScope, rScope, lValue, rValue, type, inx, AND);
}

void InterpreterObserver::or_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  bitwise(lScope, rScope, lValue, rValue, type, inx, OR);
}

void InterpreterObserver::xor_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int inx) {
  bitwise(lScope, rScope, lValue, rValue, type, inx, XOR);
}

// ***** Vector Operations ***** //

void InterpreterObserver::extractelement(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {
  DEBUG_STDOUT("Unimplemented function.");
  safe_assert(false);
}

void InterpreterObserver::insertelement() {
  DEBUG_STDOUT("Unimplemented function.");
  safe_assert(false);
}

void InterpreterObserver::shufflevector() {
  DEBUG_STDOUT("Unimplemented function.");
  safe_assert(false);
}


// ***** AGGREGATE OPERATIONS ***** //

void InterpreterObserver::extractvalue(IID iid UNUSED, int inx, int opinx) {
  //int index, count; 
  unsigned index;
  IValue *aggIValue, *iResult;
  KVALUE *aggKValue;

  // We expect only one index in the getElementPtrIndexList.
  safe_assert(getElementPtrIndexList.size() == 1);
  index = getElementPtrIndexList[0];
  getElementPtrIndexList.pop_back();


  // Obtain KVALUE and IValue objects.
  aggKValue = returnStruct[0];

  if (opinx == -1) {
    aggIValue = NULL;
  } 
  else {
    aggIValue = aggKValue->isGlobal ? globalSymbolTable[opinx] :
      executionStack.top()[opinx];
  }

  aggKValue = returnStruct[index];
  returnStruct.clear(); // in code some elements stay there

  DEBUG_STDOUT("KVALUE: " << KVALUE_ToString(aggKValue));

  // Compute the result
  iResult = new IValue();
  if (aggIValue != NULL) { 
    aggIValue += index;
    aggIValue->copy(iResult);
  } 
  else { // constant struct, use KVALUE to create iResult
    iResult->setType(aggKValue->kind);
    iResult->setValue(aggKValue->value);
  }

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = iResult;

  DEBUG_STDOUT(iResult->toString());
  return;
}

void InterpreterObserver::insertvalue(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {
  DEBUG_STDOUT("Unimplemented function.");
  safe_assert(false);
}

// ***** Memory Access and Addressing Operations ***** //

void InterpreterObserver::allocax(IID iid UNUSED, KIND type, uint64_t size UNUSED, int inx, uint64_t actualAddress) {
  // KVALUE* actualAddress
  //pre_allocax(iid, type, size, inx, line, arg, actualAddress);

  IValue *ptrLocation, *location;

  DEBUG_STDOUT("LOCAL alloca");

  // alloca for non-argument variables
  location = new IValue(type); // should we count it as LOCAL?
  ptrLocation = executionStack.top()[inx];

  VALUE value;
  value.as_ptr = (void*)actualAddress;
 
  ptrLocation->setAll(PTR_KIND, value, KIND_GetSize(type), 0, 1, (int64_t)location - (int64_t)value.as_ptr);
  ptrLocation->setScope(LOCAL);

  DEBUG_STDOUT("actual address: " << value.as_ptr);
  DEBUG_STDOUT("location" << location);
  DEBUG_STDOUT("Location: " << location->toString());
  DEBUG_STDOUT(ptrLocation->toString());

  safe_assert(ptrLocation->getValueOffset() != -1);

  //post_allocax(iid, type, size, inx, line, arg, actualAddress);
  return;
}

void InterpreterObserver::allocax_array(IID iid UNUSED, KIND type, uint64_t size, int inx, uint64_t actualAddress) {

  unsigned firstByte, bitOffset, length;

  firstByte = 0;
  bitOffset = 0;
  length = 0; 

  // if array element is struct, get list of primitive types for each struct element
  uint64_t structSize = 1;
  if (type == STRUCT_KIND) {
    structSize = structType.size(); 
  }
  KIND* structKind = new KIND[structSize];
  if (type == STRUCT_KIND) {
    for(unsigned i = 0; i < structSize; i++) {
      structKind[i] = structType[i];
    }
    structType.clear();
  }

  IValue* locArr = new IValue[size*structSize];
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
    } 
    else {
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

  VALUE value;
  value.as_ptr = (void*)actualAddress;
  IValue* locArrPtr = new IValue(PTR_KIND, value, LOCAL);
  locArrPtr->setValueOffset((int64_t)locArr - (int64_t)locArrPtr->getPtrValue());
  locArrPtr->setSize(KIND_GetSize(locArr[0].getType()));
  locArrPtr->setLength(length);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = locArrPtr;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());

  safe_assert(locArrPtr->getValueOffset() != -1);
  return;
}

void InterpreterObserver::allocax_struct(IID iid UNUSED, uint64_t size, int inx, uint64_t actualAddress) {

  safe_assert(structType.size() == size);

  unsigned firstByte = 0;
  unsigned bitOffset = 0;
  unsigned length = 0;
  IValue* ptrToStructVar = new IValue[size];

  for (unsigned i = 0; i < structType.size(); i++) {
    KIND type = structType[i];
    IValue* var = new IValue(type);
    var->setFirstByte(firstByte + bitOffset/8);
    var->setBitOffset(bitOffset%8);
    var->setLength(0);
    firstByte += KIND_GetSize(type);
    bitOffset = (type == INT1_KIND) ? bitOffset + 1 : bitOffset;
    length++;
    ptrToStructVar[i] = *var;
  }
  structType.clear();

  VALUE value;
  value.as_ptr = (void*)actualAddress;
  IValue* structPtrVar = new IValue(PTR_KIND, value);
  structPtrVar->setValueOffset((int64_t) ptrToStructVar - (int64_t) structPtrVar->getPtrValue());
  structPtrVar->setSize(KIND_GetSize(ptrToStructVar[0].getType()));
  structPtrVar->setLength(length);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = structPtrVar;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());

  safe_assert(structPtrVar->getValueOffset() != -1);
  return;
}

void InterpreterObserver::fence() {
  cerr << "[InterpreterObserver::fence] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::cmpxchg(IID iid UNUSED, PTR addr UNUSED, KVALUE* kv1 UNUSED, KVALUE* kv2 UNUSED, int inx UNUSED) {
  cerr << "[InterpreterObserver::cmpxchg] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::atomicrmw() {
  cerr << "[InterpreterObserver::atomicrmw] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::getelementptr(IID iid UNUSED, int baseInx, SCOPE baseScope, uint64_t baseAddr, int offsetInx, 
					int64_t offsetValue, KIND type, uint64_t size, bool loadGlobal, int loadInx, int inx) {

  if (type == INT80_KIND) {
    DEBUG_STDERR("[getelementptr] Unsupported INT80_KIND");
    safe_assert(false);
    return; // otherwise compiler warning
  }

  IValue *basePtrLocation, *ptrLocation, *array; 
  int length, index, actualOffset;
  bool reInit;

  // retrieving base pointer operand
  if (baseInx == -1) { // constant base pointer
    VALUE value;
    value.as_ptr = (void*)baseAddr;
    basePtrLocation = new IValue(PTR_KIND, value, 0, 0, 0, 0);
  } 
  else {    
    if (baseScope == GLOBAL) {
      basePtrLocation = globalSymbolTable[baseInx];
    }
    else {
      basePtrLocation = executionStack.top()[baseInx];
    }
  }
  DEBUG_STDOUT("\tBase pointer operand " << basePtrLocation->toString());
  // done retriving base pointer operand

  // retrieving index operand
  index = offsetInx == -1 ? offsetValue :
    executionStack.top()[offsetInx]->getValue().as_int; 
  DEBUG_STDOUT("\tIndex value: " << index);

  // computing actual offset from base pointer in bytes 
  actualOffset = (index * (size/8)) + basePtrLocation->getOffset();
  DEBUG_STDOUT("\tactualOffset: " << actualOffset);

  DEBUG_STDOUT("\tSize: " << size);
  DEBUG_STDOUT("\tBase Offset: " << basePtrLocation->getOffset());

  // retriving first element pointed to and number of elements (> 1 for actual arrays)
  array = (IValue*) basePtrLocation->getIPtrValue();
  length = basePtrLocation->getLength();
  //originalSize = KIND_GetSize(array[0].getType()); // NEW

  // checking whether the base pointer needs to be (re)initialized.
  reInit = !basePtrLocation->isInitialized();

  // additional check for out of bounds when initialized
  if (!reInit ) {
    //if (actualOffset < 0 || actualOffset + size/8 > array[length-1].getFirstByte() + KIND_GetSize(array[length-1].getType())) { 
    if (actualOffset < 0 || actualOffset + size/8 > array[length-1].getFirstByte() + basePtrLocation->getSize()) { // same original size for all elements?
      reInit = true;
    }
  }

  // (re)initializing the base pointer if neccessary
  if (reInit) {
    DEBUG_STDERR("\tPointer is re-initialized!");
    DEBUG_STDOUT("\tPointer is re-initialized!");

    IValue *newArray, *loadInst;
    int newLength, extraBytes;

    // determining new length of array
    if (actualOffset >= 0) { // actualOffset is positive
      if (basePtrLocation->isInitialized()) {
        //extraBytes = actualOffset + size/8 - array[length-1].getFirstByte() - KIND_GetSize(array[length-1].getType()); 
        extraBytes = actualOffset + size/8 - array[length-1].getFirstByte() - basePtrLocation->getSize(); 
      } 
      else {
        extraBytes = actualOffset + size/8;
      }
    } 
    else { // actualOffset is negative
      DEBUG_STDOUT("Actual offset is negative.");
      extraBytes = abs(actualOffset);
    }
    DEBUG_STDOUT("Extra bytes: " << extraBytes);
    newLength = length + ceil((double)extraBytes/(double)(size/8));

    // optimization: allocate twice more space than required
    newLength = index >= 0 ? 2 * newLength : newLength;

    DEBUG_STDOUT("Old length: " << length);
    DEBUG_STDOUT("New length: " << newLength);

    // allocating and popularing new array
    newArray = new IValue[newLength];

    unsigned firstByte = 0;
    unsigned originalSize = basePtrLocation->getSize(); // new

    for (int i = 0; i < newLength; i++) {
      IValue oldElement; 
      VALUE value;
      value.as_int = 0;

      if (actualOffset < 0) { // actualOffset is negative, append new elements at the beginning of the array

        if (i < newLength - length) {
	  newArray[i].setTypeValue(type, value);
        } 
	else {
          oldElement = array[i + length - newLength];
          oldElement.copy(&newArray[i]);
        }
	newArray[i].setFirstByte(firstByte);
	firstByte += originalSize;
      } 
      else { // actualOffset is positive, append at the end of the array new element
        if (i < length) {
          oldElement = array[i];
          oldElement.copy(&newArray[i]);
          newArray[i].setFirstByte(oldElement.getFirstByte());
        } 
	else {
	  newArray[i].setTypeValue(type, value);
	  newArray[i].setFirstByte(firstByte);
	  firstByte += originalSize;
        }
      }

      DEBUG_STDOUT("\tNew element at index " << i << " is: " << newArray[i].toString());
    }

    basePtrLocation->setLength(newLength);
    basePtrLocation->setSize(size/8); // REVISE: I thought this would be the original size instead
    basePtrLocation->setValueOffset((int64_t) newArray - basePtrLocation->getValue().as_int);

    // update load variable
    if (loadInx != -1) {
      IValue *elem, *values;

      // TODO: load can also be a global variable
      loadInst = loadGlobal ? globalSymbolTable[loadInx] : executionStack.top()[loadInx];

      // retrieving source
      values = (IValue*)loadInst->getIPtrValue();
      elem = values + loadInst->getIndex();
      elem->setLength(basePtrLocation->getLength());
      elem->setSize(basePtrLocation->getSize());
      elem->setValueOffset(basePtrLocation->getValueOffset());
    }

    // delete here
    for(int i = length - 1; i >= 0; i--) {
      array[i].IValue::~IValue();
    }
  }
  // done (re)initializing the base pointer

  // common case: pointer to array is not cast (use firstByte and type size to verify this)
  // otherwise, call findIndex to get the actual index

  array = (IValue*) basePtrLocation->getIPtrValue();
  index = index + basePtrLocation->getIndex();

  safe_assert(index < (int) basePtrLocation->getLength());
  if (index < 0 || (int) array[index].getFirstByte() != index*(int)size/8 || KIND_GetSize(array[index].getType()) != (int) size/8) {
    index = findIndex(array, actualOffset, basePtrLocation->getLength()); 
  } 

  ptrLocation = executionStack.top()[inx];
  ptrLocation->setAll(PTR_KIND, basePtrLocation->getValue(), size/8, /*actualOffset, */index, basePtrLocation->getLength(), basePtrLocation->getValueOffset());
  ptrLocation->setOffset(actualOffset);

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  return;
}

void InterpreterObserver::getelementptr_array(int baseInx, SCOPE baseScope, uint64_t baseAddr, int elementSize, 
					      int scopeInx01, int scopeInx02, int scopeInx03, int64_t valOrInx01, int64_t valOrInx02, 
					      int64_t valOrInx03, int size01 UNUSED, int size02,  int inx) {

  IValue* arrayElemPtr;
  int newOffset;

  if (baseInx == -1) {
    // TODO: review this
    // constant pointer
    // return a dummy object
    VALUE value;
    value.as_ptr = (void*)baseAddr;
    arrayElemPtr = new IValue(PTR_KIND, value, 0, 0, 0, 0);

    getElementPtrIndexList.clear();
    arraySize.clear();
  } 
  else {
    IValue *ptrArray, *array;
    int *arraySizeVec, *indexVec;
    int index, arrayDim;
    unsigned getIndexNo;

    if (baseScope == GLOBAL) {
      ptrArray = globalSymbolTable[baseInx];
    }
    else {
      ptrArray = executionStack.top()[baseInx];
    }
    array = static_cast<IValue*>(ptrArray->getIPtrValue());

    DEBUG_STDOUT("\tPointer operand: " << ptrArray->toString());

    // compute the index for flatten array representation of
    // the program's multi-dimensional array
    arrayDim = (size02 != -1) ? arraySize.size() + 2 : 1;

    if (scopeInx02 == SCOPE_INVALID) {
      scopeInx02 = CONSTANT;
      valOrInx02 = 0;
    } 

    if (scopeInx03 == SCOPE_INVALID) {
      getIndexNo = 1;
    } 
    else {
      getIndexNo = getElementPtrIndexList.size() + 2;
    }

    DEBUG_STDOUT("arrayDim " << arrayDim);
    DEBUG_STDOUT("getIndexNo " << getIndexNo);

    safe_assert(getIndexNo != 0);

    arraySizeVec = new int[getIndexNo];
    indexVec = new int[getIndexNo];

    // the size of out-most dimension; 
    // we do not need this to compute the index
    if (size02 != -1) {
      arraySizeVec[0] = size02;

      for(unsigned i = 1; i < arraySize.size(); i++) {
	if (i < getIndexNo) {
	  arraySizeVec[i] = arraySize[i - 1];
	}
      }
      arraySize.clear();
    }

    arraySizeVec[getIndexNo-1] = 1; 

    for (unsigned i = 0; i < getIndexNo; i++) {
      for (unsigned j = i+1; j < getIndexNo; j++) {
        arraySizeVec[i] *= arraySizeVec[j]; 
      }
    }

    // the first index is for the pointer operand;
    array = array + actualValueToIntValue(scopeInx01, valOrInx01);
    safe_assert(scopeInx01 != SCOPE_INVALID);
    indexVec[0] = actualValueToIntValue(scopeInx02, valOrInx02);

    if (scopeInx03 != SCOPE_INVALID) {
      indexVec[1] = actualValueToIntValue(scopeInx03, valOrInx03);
      unsigned i = 2;

      for(unsigned int j = 0; j < getElementPtrIndexList.size(); j++) {
	indexVec[i + j] = getElementPtrIndexList[j];
      }
      getElementPtrIndexList.clear();
    }

    index = 0;
    for (unsigned i = 0; i < getIndexNo; i++) {
      index += indexVec[i] * arraySizeVec[i];
    }

    delete[] arraySizeVec;
    delete[] indexVec;
    DEBUG_STDOUT("\tIndex: " << index);

    // compute new offset for flatten array
    newOffset = ptrArray->getOffset() + elementSize*index; 

    // compute the index for the casted fatten array
    if (ptrArray->isInitialized()) {
      index = findIndex((IValue*) ptrArray->getIPtrValue(), newOffset, ptrArray->getLength()); 
    }

    DEBUG_STDOUT("\tIndex: " << index);

    // TODO: revisit this
    if (index < (int) ptrArray->getLength()) {
      IValue* arrayElem = array + index;
      arrayElemPtr = new IValue(PTR_KIND, ptrArray->getValue());
      arrayElemPtr->setValueOffset(ptrArray->getValueOffset());
      arrayElemPtr->setIndex(index);
      arrayElemPtr->setLength(ptrArray->getLength());
      arrayElemPtr->setSize(KIND_GetSize(arrayElem[0].getType()));
      arrayElemPtr->setOffset(arrayElem[0].getFirstByte());
    } 
    else {
      VALUE arrayElemPtrValue;
      arrayElemPtrValue.as_int = ptrArray->getValue().as_int + newOffset;
      arrayElemPtr = new IValue(PTR_KIND, arrayElemPtrValue, ptrArray->getSize(), 0, 0, 0);
      arrayElemPtr->setValueOffset((int64_t)arrayElemPtr - arrayElemPtr->getValue().as_int);
    }
  }

  safe_assert(getElementPtrIndexList.empty());

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = arrayElemPtr;
  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  return;
}

void InterpreterObserver::getelementptr_struct(IID iid UNUSED, int baseInx, SCOPE baseScope, uint64_t baseAddr, int inx) {

  DEBUG_STDOUT("\tstructType size " << structType.size());

  IValue *structPtr, *structElemPtr; 
  int structElemNo, structSize, index, size, newOffset;
  int* structElemSize, *structElem;

  if (baseInx == -1) {
    // TODO: review this
    // constant pointer
    // return a dummy object
    VALUE value;
    value.as_ptr = (void*)baseAddr;
    structElemPtr = new IValue(PTR_KIND, value, 0, 0, 0, 0);

    getElementPtrIndexList.clear();

    while (!structElementSize.empty()) {
      structElementSize.pop();
    }
  } 
  else {
    // get the struct operand
    if (baseScope == GLOBAL) {
      structPtr = globalSymbolTable[baseInx];
    }
    else {
      structPtr = executionStack.top()[baseInx];
    }
    structElemNo = structType.size();
    
    structElemSize = new int[structElemNo];
    structElem = new int[structElemNo];

    // record struct element size
    // compute struct size
    structSize = 0;

    for(unsigned i = 0; i < structType.size(); i++) {
      structElemSize[i] = KIND_GetSize(structType[i]);
      structElem[i] = structType[i];
      structSize += structElemSize[i];
    }
    structType.clear();


    DEBUG_STDOUT("\tstructSize is " << structSize);

    DEBUG_STDOUT("\t" << structPtr->toString());

    // compute struct index
    DEBUG_STDOUT("\tsize of getElementPtrIndexList: " << getElementPtrIndexList.size());

    index = getElementPtrIndexList[0] * structElemNo;
    if (getElementPtrIndexList.size() > 1) {
      for(unsigned int i = 0; i < getElementPtrIndexList[1]; i++) {
	index = index + structElementSize.front();
	safe_assert(!structElementSize.empty());
	structElementSize.pop();  
      }
    }
    getElementPtrIndexList.clear();
    
    while (!structElementSize.empty()) {
      structElementSize.pop();
    }
    safe_assert(getElementPtrIndexList.empty());

    DEBUG_STDOUT("\tIndex is " << index);

    newOffset = structSize * (index/structElemNo);
    for (int i = 0; i < index % structElemNo; i++) {
      newOffset = newOffset + KIND_GetSize(structElem[i]);
    }

    newOffset = newOffset + structPtr->getOffset();
    size = KIND_GetSize(structElem[index % structElemNo]);

    DEBUG_STDOUT("\tNew offset is: " << newOffset);

    // compute the result; consider two cases: the struct pointer operand is
    // initialized and is not initialized
    if (structPtr->isInitialized()) {

      IValue* structBase = static_cast<IValue*>(structPtr->getIPtrValue());
      index = findIndex((IValue*) structPtr->getIPtrValue(), newOffset, structPtr->getLength()); // TODO: revise offset, getValue().as_ptr

      DEBUG_STDOUT("\tNew index is: " << index);

      // TODO: revisit this
      if (index < (int) structPtr->getLength()) {
        DEBUG_STDOUT("\tstructBase = " << structBase->toString());
        IValue* structElem = structBase + index;
        DEBUG_STDOUT("\tstructElem = " << structElem->toString());
        structElemPtr = new IValue(PTR_KIND, structPtr->getValue());
        structElemPtr->setValueOffset(structPtr->getValueOffset());
        structElemPtr->setIndex(index);
        structElemPtr->setLength(structPtr->getLength());
        structElemPtr->setSize(size);
        structElemPtr->setOffset(newOffset);
      } 
      else {
        structElemPtr = new IValue(PTR_KIND, structPtr->getValue(), structPtr->getSize(), 0, 0, 0);
        structElemPtr->setValueOffset(structPtr->getValueOffset());
      }
    } 
    else {
      DEBUG_STDOUT("\tPointer is not initialized");
      VALUE structElemPtrValue;

      // compute the value for the element pointer
      structElemPtrValue = structPtr->getValue();
      structElemPtrValue.as_int = structElemPtrValue.as_int + newOffset;

      structElemPtr = new IValue(PTR_KIND, structElemPtrValue, size, 0, 0, 0);
      structElemPtr->setValueOffset((int64_t)structElemPtr - structElemPtr->getValue().as_int);
    }
  }

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = structElemPtr;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  return;
}

// ***** Conversion Operations ***** //

void InterpreterObserver::castop(int64_t opVal, SCOPE opScope, KIND opType, KIND type, int size, int inx, CASTOP op) {
  VALUE result;
  IValue *iOp, *iResult;
  int64_t v64, opIntValue, opPtrValue;
  uint64_t opUIntValue;
  int64_t *v64Ptr;
  int32_t v32, sign;
  double opFlpValue;

  // assert: we do not support INT80 yet.
  if (type == INT80_KIND || opType == INT80_KIND) {
    DEBUG_STDERR("Do not support INT80 type yet.");
    safe_assert(false);
  }

  // Obtain value and type of the operand.
  if (opScope == CONSTANT) {
    double *ptr = (double *)&op;
    iOp = NULL; // compiler warning without this

    opIntValue = op;
    opUIntValue = op;
    opFlpValue = *ptr;
    opPtrValue = op;

  } 
  else {

    iOp = (opScope == GLOBAL) ? globalSymbolTable[opVal] :
      executionStack.top()[opVal];
    opIntValue = iOp->getIntValue();
    opUIntValue = iOp->getUIntValue();
    opFlpValue = iOp->getFlpValue();
    opPtrValue = iOp->getValue().as_int + iOp->getOffset();
  }

  // Compute 64-bit, 32-bit and sign representation of value. 
  v64 = opIntValue;
  v64Ptr = &v64;
  v32 = *((int32_t *) v64Ptr);
  sign = v32 & 0x1;

  // Compute the result based on castop and type
  switch (op) {
    case TRUNC:
      // assert: size of opType is larger than or equal to (result) type.
      safe_assert(KIND_GetSize(opType) >= KIND_GetSize(type));
      switch (type) {
        case INT1_KIND:
          result.as_int = sign;
          break;
        case INT8_KIND:
          result.as_int = v32 & 0x000000FF;
          break;
        case INT16_KIND:
          result.as_int = v32 & 0x0000FFFF;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = v32;
          break;
        case INT64_KIND:
          result.as_int = v64;
          break;
        default:
          DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case ZEXT:
      // assert: size of opType is smaller than or equal to (result) type.
      safe_assert(KIND_GetSize(opType) <= KIND_GetSize(type));
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
        case INT16_KIND:
        case INT24_KIND:
        case INT32_KIND:
        case INT64_KIND:
          result.as_int = v64;
          break;
        default:
          DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
      }
      break;

    case SEXT:
      // assert: size of opType is smaller than or equal to (result) type.
      safe_assert(KIND_GetSize(opType) <= KIND_GetSize(type));
      switch (type) {
        case INT1_KIND:
        case INT8_KIND:
        case INT16_KIND:
        case INT24_KIND:
        case INT32_KIND:
        case INT64_KIND:
          DEBUG_STDERR("Sign extension is not completely implemented yet!");
          result.as_int = v64;
          break;
        default:
          DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case FPTRUNC:
      // assert: size of opType is larger than or equal to (result) type.
      safe_assert(KIND_GetSize(opType) >= KIND_GetSize(type));
      switch (type) {
        case FLP32_KIND:
          result.as_flp = (float) opFlpValue;
          break;
        case FLP64_KIND:
          result.as_flp = (double) opFlpValue;
          break;
        case FLP80X86_KIND:
          result.as_flp = (double) opFlpValue;
          break;
        default:
          DEBUG_STDERR("Unsupported float type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case FPEXT:
      // assert: size of opType is smaller than or equal to (result) type.
      safe_assert(KIND_GetSize(opType) <= KIND_GetSize(type));
      switch (type) {
        case FLP32_KIND:
          result.as_flp = (float) opFlpValue;
          break;
        case FLP64_KIND:
          result.as_flp = (double) opFlpValue;
          break;
        case FLP80X86_KIND:
          result.as_flp = (double) opFlpValue;
          break;
        default:
          DEBUG_STDERR("Unsupported float type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;
    case FPTOUI:
      switch (type) {
        case INT1_KIND:
          result.as_int = (bool) opFlpValue;
          break;
        case INT8_KIND:
          result.as_int = (uint8_t) opFlpValue;
          break;
        case INT16_KIND:
          result.as_int = (uint16_t) opFlpValue;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = (uint32_t) opFlpValue;
          break;
        case INT64_KIND:
          result.as_int = (uint64_t) opFlpValue;
          break;
        default:
          DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case FPTOSI:
      switch (type) {
        case INT1_KIND:
          result.as_int = (bool) opFlpValue;
          break;
        case INT8_KIND:
          result.as_int = (int8_t) opFlpValue;
          break;
        case INT16_KIND:
          result.as_int = (int16_t) opFlpValue;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = (int32_t) opFlpValue;
          break;
        case INT64_KIND:
          result.as_int = (int64_t) opFlpValue;
          break;
        default:
          DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case UITOFP:
      switch (type) {
        case FLP32_KIND:
          result.as_flp = (float) opUIntValue;
          break;
        case FLP64_KIND:
          result.as_flp = (double) opUIntValue;
          break;
        case FLP80X86_KIND:
          result.as_flp = (double) opUIntValue;
          break;
        default:
          DEBUG_STDERR("Unsupported float type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case SITOFP:
      switch (type) {
        case FLP32_KIND:
          result.as_flp = (float) opIntValue;
          break;
        case FLP64_KIND:
          result.as_flp = (double) opIntValue;
          break;
        case FLP80X86_KIND:
          result.as_flp = (double) opIntValue;
          break;
        default:
          DEBUG_STDERR("Unsupported float type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case PTRTOINT:
      switch (type) {
        case INT1_KIND:
          result.as_int = (bool) opPtrValue;
          break;
        case INT8_KIND:
          result.as_int = (int8_t) opPtrValue;
          break;
        case INT16_KIND:
          result.as_int = (int16_t) opPtrValue;
          break;
        case INT24_KIND:
        case INT32_KIND:
          result.as_int = (int32_t) opPtrValue;
          break;
        case INT64_KIND:
          result.as_int = opPtrValue;
          break;
        default:
          DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
          safe_assert(false);
      }
      break;

    case INTTOPTR:
      result.as_int = opIntValue;
      break;

    case BITCAST:
      break;

    default:
      DEBUG_STDERR("Unsupported conversion operator: " << CASTOP_ToString(op));
      safe_assert(false);
  }

  iResult = executionStack.top()[inx];

  if (op == BITCAST) {
    if (opScope == CONSTANT) {
      VALUE val;
      val.as_int = op;

      iResult->clear();
      iResult->setTypeValueSize(type, val, size/8);
    } 
    else {
      iOp->copy(iResult);
      iResult->setSize(size/8);
      iResult->setType(type);
    }
  } 
  else {
    iResult->clear();
    iResult->setTypeValue(type, result);
  }

  DEBUG_STDOUT(iResult->toString());
  return;
}

void InterpreterObserver::trunc(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, TRUNC);
}

void InterpreterObserver::zext(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, ZEXT);
}

void InterpreterObserver::sext(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, SEXT);
}

void InterpreterObserver::fptrunc(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  //pre_fptrunc(opVal, opScope, opKind, kind, size, inx);
  castop(opVal, opScope, opKind, kind, size, inx, FPTRUNC);
  //post_fptrunc(opVal, opScope, opKind, kind, size, inx);
}

void InterpreterObserver::fpext(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  //pre_fpext(opVal, opScope, opKind, kind, size, inx);
  castop(opVal, opScope, opKind, kind, size, inx, FPEXT);
  //post_fpext(opVal, opScope, opKind, kind, size, inx);
}

void InterpreterObserver::fptoui(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  //pre_fptoui(opVal, opScope, opKind, kind, size, inx);
  castop(opVal, opScope, opKind, kind, size, inx, FPTOUI);
  //post_fptoui(opVal, opScope, opKind, kind, size, inx);
}

void InterpreterObserver::fptosi(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  //pre_fptosi(opVal, opScope, opKind, kind, size, inx);
  castop(opVal, opScope, opKind, kind, size, inx, FPTOSI);
  //post_fptosi(opVal, opScope, opKind, kind, size, inx);
}

void InterpreterObserver::uitofp(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, UITOFP);
}

void InterpreterObserver::sitofp(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, SITOFP);
}

void InterpreterObserver::ptrtoint(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, PTRTOINT);
}

void InterpreterObserver::inttoptr(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, INTTOPTR);
}

void InterpreterObserver::bitcast(int64_t opVal, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  castop(opVal, opScope, opKind, kind, size, inx, BITCAST);
}

// ***** TerminatorInst ***** //
void InterpreterObserver::branch(IID iid UNUSED, bool conditional UNUSED, int valInx, SCOPE scope UNUSED, KIND type UNUSED, uint64_t value) {

  // TODO: how about the SCOPE == GLOBAL?

  IValue* cond = (valInx == -1) ? NULL : executionStack.top()[valInx];

  if (cond != NULL && ((bool) cond->getIntValue() != (bool)value)) { // revise this: before value.as_int
    DEBUG_STDERR("\tKVALUE: " << "inx: " << valInx << ", scope: " << SCOPE_ToString(scope) << ", type: " << KIND_ToString(type) << ", value: " << value);
    DEBUG_STDERR("\tIVALUE: " << cond->toString());

    DEBUG_STDERR("\tShadow and concrete executions diverge at this branch.");
    safe_assert(false);
  }
  return;
}

void InterpreterObserver::branch2(IID iid UNUSED, bool conditional UNUSED) {
}

void InterpreterObserver::indirectbr(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {
}

void InterpreterObserver::invoke(IID iid UNUSED, KVALUE* call_value UNUSED, int inx UNUSED) {
  int count; 

  count = 0;
  while (!myStack.empty()) {
    KVALUE* argument; 

    argument = myStack.top();
    DEBUG_STDOUT("\t Argument " << count << ": " << KVALUE_ToString(argument));
    myStack.pop();
  }

  DEBUG_STDERR("Unimplemented function.");
  safe_assert(false);
}

void InterpreterObserver::resume(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {
  DEBUG_STDERR("Unimplemented function.");
  safe_assert(false);
}

void InterpreterObserver::return_(IID iid UNUSED, int valInx, SCOPE scope UNUSED, KIND type, int64_t value) {
  safe_assert(!executionStack.empty());

  std::vector< IValue* > iValues;
  iValues = executionStack.top();

  IValue* returnValue = valInx == -1 ? NULL : executionStack.top()[valInx];

  executionStack.pop();

  if (!executionStack.empty()) {
    DEBUG_STDOUT("New stack size: " << executionStack.size());
    safe_assert(!callerVarIndex.empty());

    if (returnValue == NULL) {
      executionStack.top()[callerVarIndex.top()]->setType(type); 
      executionStack.top()[callerVarIndex.top()]->setValue(value);
    } else {
      returnValue->copy(executionStack.top()[callerVarIndex.top()]);
    }
    DEBUG_STDOUT(executionStack.top()[callerVarIndex.top()]->toString());

    callerVarIndex.pop();
  } else {
    cout << "The execution stack is empty.\n";

    post_analysis();
  }

  // free memory
  // should not be erasing above stuff twice
  


  for (unsigned int i = 0; i < iValues.size(); i++) {
    release(iValues[i]);
  }
  IValue::printCounters();

  /*
  for(unsigned int i = collect_new.size() - 1; i <= collect_new.size(); i++) {
    delete[] collect_new[i];
    collect_new.pop_back();
  }

  for(unsigned int i = collect_malloc.size() - 1; i <= collect_malloc.size(); i++) {
    free(collect_malloc[i]);
    collect_malloc.pop_back();
  }
  */

  isReturn = true;
  return;
}

void InterpreterObserver::return2_(IID iid UNUSED, int inx UNUSED) {

  safe_assert(!executionStack.empty());

  std::vector< IValue* > iValues;
  iValues = executionStack.top();

  executionStack.pop();

  if (!executionStack.empty()) {
    DEBUG_STDOUT("New stack size: " << executionStack.size());
  } else {
    cout << "The execution stack is empty.\n";
  }



  // freeing memory
  for (unsigned int i = 0; i < iValues.size(); i++) {
    release(iValues[i]);
  }

  IValue::printCounters();
  isReturn = true;
  return;
}

void InterpreterObserver::return_struct_(IID iid UNUSED, int inx UNUSED, int valInx) {

  safe_assert(!executionStack.empty());

  std::vector< IValue* > iValues;
  iValues = executionStack.top();

  IValue* returnValue = (valInx == -1) ? NULL : executionStack.top()[valInx];

  executionStack.pop();

  if (!executionStack.empty()) {
    DEBUG_STDOUT("New stack size: " << executionStack.size());
    safe_assert(!callerVarIndex.empty());
    safe_assert(!returnStruct.empty());

    // reconstruct struct value
    unsigned structSize = returnStruct.size();
    IValue* structValue = new IValue[structSize];
    
    for(unsigned i = 0; i < structSize; i++) {
      KVALUE* value = returnStruct[i];

      if (returnValue == NULL) {
	structValue[i].setType(value->kind);
	structValue[i].setValue(value->value);
	structValue[i].setLength(0);
      } 
      else {
	returnValue->copy(&structValue[i]);
        returnValue++;
      }

      DEBUG_STDOUT(cout << structValue[i].toString());
    }
    returnStruct.clear();

    structValue->setStruct(true);

    executionStack.top()[callerVarIndex.top()] = structValue;
    /*
    for (i = 0; i < size; i++) {
      DEBUG_STDOUT(executionStack.top()[callerVarIndex.top()][i].toString());
    }
    */
  } 
  else {
    cout << "The execution stack is empty.\n";
  }

  safe_assert(!callerVarIndex.empty());
  callerVarIndex.pop();


 
 // freeing memory
  for (unsigned int i = 0; i < iValues.size(); i++) {
    if (i != (unsigned)valInx) { // TODO: do not delete struct from now, make copy first!
      release(iValues[i]);
    }
  }

  IValue::printCounters();
  isReturn = true;
  return;
}

void InterpreterObserver::switch_(IID iid UNUSED, KVALUE* op UNUSED, int inx UNUSED) {
}

void InterpreterObserver::unreachable() {
  safe_assert(false);
}

// ***** Other Operations ***** //

void InterpreterObserver::icmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, PRED pred, int inx) {
  if (type == INT80_KIND) {
    cout << "[icmp] Unsupported INT80_KIND" << endl;
    safe_assert(false);
  }

  int64_t v1, v2;

  // get value of v1
  if (lScope == CONSTANT) { // constant
    v1 = lValue;
  } 
  else {
    IValue *loc1 = (lScope == GLOBAL) ? globalSymbolTable[lValue] :
      executionStack.top()[lValue];
    v1 = loc1->getType() == PTR_KIND ? loc1->getIntValue() + loc1->getOffset()
      : loc1->getIntValue();
  }

  // get value of v2
  if (rScope == CONSTANT) { // constant
    v2 = rValue;
  } 
  else {
    IValue *loc2 = (rScope == GLOBAL) ? globalSymbolTable[rValue] :
      executionStack.top()[rValue];
    v2 = loc2->getType() == PTR_KIND ? loc2->getIntValue() + loc2->getOffset()
      : loc2->getIntValue();
  } 

  DEBUG_STDOUT("=============" << v1);
  DEBUG_STDOUT("=============" << v2);

  VALUE result;
  switch(pred) {
    case CmpInst::ICMP_EQ:
      DEBUG_STDOUT("PRED = ICMP_EQ");
      result.as_int = v1 == v2;
      break;
    case CmpInst::ICMP_NE:
      DEBUG_STDOUT("PRED = ICMP_NE");
      result.as_int = v1 != v2;
      break;
    case CmpInst::ICMP_UGT:
      DEBUG_STDOUT("PRED = ICMP_UGT");
      result.as_int = (uint64_t)v1 > (uint64_t)v2;
      break;
    case CmpInst::ICMP_UGE:
      DEBUG_STDOUT("PRED = ICMP_UGE");
      result.as_int = (uint64_t)v1 >= (uint64_t)v2;
      break;
    case CmpInst::ICMP_ULT:
      DEBUG_STDOUT("PRED = ICMP_ULT");
      result.as_int = (uint64_t)v1 < (uint64_t)v2;
      break;
    case CmpInst::ICMP_ULE:
      DEBUG_STDOUT("PRED = ICMP_ULE");
      result.as_int = (uint64_t)v1 <= (uint64_t)v2;
      break;
    case CmpInst::ICMP_SGT:
      DEBUG_STDOUT("PRED = ICMP_SGT");
      result.as_int = v1 > v2;
      break;
    case CmpInst::ICMP_SGE:
      DEBUG_STDOUT("PRED = ICMP_SGE");
      result.as_int = v1 >= v2;
      break;
    case CmpInst::ICMP_SLT:
      DEBUG_STDOUT("PRED = ICMP_SLT");
      result.as_int = v1 < v2;
      break;
    case CmpInst::ICMP_SLE:
      DEBUG_STDOUT("PRED = ICMP_SLE");
      result.as_int = v1 <= v2;
      break;
    default:
      safe_assert(false);
      break;
  }

  IValue *iResult = executionStack.top()[inx];
  iResult->clear();
  iResult->setTypeValue(INT1_KIND, result);
  iResult->setSize(0); // size for INT1_KIND
  
  DEBUG_STDOUT(iResult->toString());
  return;
}

void InterpreterObserver::fcmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type UNUSED, PRED pred, int inx) {
  //pre_fcmp(lScope, rScope, lValue, rValue, type, pred, line, inx);

  double v1, v2;
  VALUE result;

  // get value of v1
  if (lScope == CONSTANT) { // constant
    double *ptr= (double *) &lValue;
    v1 = *ptr;
  } 
  else {
    IValue *loc1 = (lScope == GLOBAL) ? globalSymbolTable[lValue] :
      executionStack.top()[lValue];
    v1 = loc1->getFlpValue();
  } 

  // get value of v2
  if (rScope == CONSTANT) { // constant
    double *ptr = (double *) &rValue;
    v2 = *ptr;
  } 
  else {
    IValue *loc2 = (rScope == GLOBAL) ? globalSymbolTable[rValue] :
      executionStack.top()[rValue];
    v2 = loc2->getFlpValue();
  } 

  DEBUG_STDOUT("=============" << v1);
  DEBUG_STDOUT("=============" << v2);

  switch(pred) {
    case CmpInst::FCMP_FALSE:
      DEBUG_STDOUT("\tCondition is FALSE");
      result.as_int = 0;
      break;
    case CmpInst::FCMP_TRUE:
      DEBUG_STDOUT("\tCondition is TRUE");
      result.as_int = 1;
      break;
    case CmpInst::FCMP_UEQ:
      DEBUG_STDOUT("\tCondition is UEQ");
      result.as_int = v1 == v2;
      break;
    case CmpInst::FCMP_UNE:
      DEBUG_STDOUT("\tCondition is UNE");
      result.as_int = v1 != v2;
      break;
    case CmpInst::FCMP_UGT:
      DEBUG_STDOUT("\tCondition is UGT");
      result.as_int = v1 > v2;
      break;
    case CmpInst::FCMP_UGE:
      DEBUG_STDOUT("\tCondition is UGE");
      result.as_int = v1 >= v2;
      break;
    case CmpInst::FCMP_ULT:
      DEBUG_STDOUT("\tCondition is ULT");
      result.as_int = v1 < v2;
      break;
    case CmpInst::FCMP_ULE:
      DEBUG_STDOUT("\tCondition is ULT");
      result.as_int = v1 <= v2;
      break;
    case CmpInst::FCMP_OEQ:
      DEBUG_STDOUT("\tCondition is OEQ");
      result.as_int = v1 == v2;
      break;
    case CmpInst::FCMP_ONE:
      DEBUG_STDOUT("\tCondition is ONE"); 
      result.as_int = v1 != v2;
      break;
    case CmpInst::FCMP_OGT:
      DEBUG_STDOUT("\tCondition is OGT");
      result.as_int = v1 > v2;
      break;
    case CmpInst::FCMP_OGE:
      DEBUG_STDOUT("\tCondition is OGE");
      result.as_int = v1 >= v2;
      break;
    case CmpInst::FCMP_OLT:
      DEBUG_STDOUT("\tCondition is OLT");
      result.as_int = v1 < v2;
      break;
    case CmpInst::FCMP_OLE:
      DEBUG_STDOUT("\tCondition is OLE");
      result.as_int = v1 <= v2;
      break;
    default:
      safe_assert(false);
      break;
  }

  IValue* iResult = executionStack.top()[inx];
  iResult->clear();
  iResult->setTypeValue(INT1_KIND, result);
  DEBUG_STDOUT(iResult->toString());

  //post_fcmp(lScope, rScope, lValue, rValue, type, pred, line, inx);
  return;
}

void InterpreterObserver::phinode(IID iid UNUSED, int inx) {

  DEBUG_STDOUT("Recent block: " << recentBlock.top());

  IValue* phiNode;

  if (phinodeConstantValues.find(recentBlock.top()) != phinodeConstantValues.end()) {
    KVALUE* constant = phinodeConstantValues[recentBlock.top()];
    phiNode = new IValue(constant->kind, constant->value);
    phiNode->setLength(0);
  }
  else {
    safe_assert(phinodeValues.find(recentBlock.top()) != phinodeValues.end());
    IValue* inValue = executionStack.top()[phinodeValues[recentBlock.top()]];
    phiNode = new IValue();
    inValue->copy(phiNode);
  }

  phinodeConstantValues.clear();
  phinodeValues.clear();

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = phiNode;

  DEBUG_STDOUT(phiNode->toString());
  return;
}

void InterpreterObserver::select(IID iid UNUSED, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {

  int condition;
  IValue *conditionValue, *trueValue, *falseValue, *result;

  if (cond->inx == -1) {
    condition = cond->value.as_int;
  } 
  else {
    conditionValue = cond->isGlobal ? globalSymbolTable[cond->inx] : executionStack.top()[cond->inx];
    condition = conditionValue->getValue().as_int;
  }


  if (condition) {
    if (tvalue->inx == -1) {
      result = new IValue(tvalue->kind, tvalue->value, REGISTER);
    } 
    else {
      result = new IValue();
      trueValue = tvalue->isGlobal ? globalSymbolTable[tvalue->inx] :
        executionStack.top()[tvalue->inx];
      trueValue->copy(result);
    }
  } 
  else {
    if (fvalue->inx == -1) {
      result = new IValue(fvalue->kind, fvalue->value, REGISTER);
    } 
    else {
      result = new IValue();
      falseValue = fvalue->isGlobal ? globalSymbolTable[fvalue->inx] : executionStack.top()[fvalue->inx];
      falseValue->copy(result);
    }
  }

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = result;

  DEBUG_STDOUT("Result is " << result->toString());
  return;
}

void InterpreterObserver::push_string(int diff) {
  char c = (char)(((int)'0') + diff);
  logName.push(c);
  return;
}

void InterpreterObserver::push_stack(int inx, SCOPE scope, KIND type, uint64_t addr) {
  KVALUE* kvalue = new KVALUE();
  kvalue->inx = inx;
  if (scope == GLOBAL) {
    kvalue->isGlobal = true;
  }
  else {
    kvalue->isGlobal = false;
  }
  kvalue->kind = type;
  kvalue->value.as_ptr = (void*)addr;
  myStack.push(kvalue);
  return;
}

void InterpreterObserver::push_phinode_constant_value(KVALUE* value, int blockId) {
  phinodeConstantValues[blockId] = value;
  return;
}

void InterpreterObserver::push_phinode_value(int valId, int blockId) {
  phinodeValues[blockId] = valId;
  return;
}

void InterpreterObserver::push_return_struct(KVALUE* value) {
  returnStruct.push_back(value);
  return;
}

void InterpreterObserver::push_struct_type(KIND kind) {
  structType.push_back(kind);
  return;
}

void InterpreterObserver::push_struct_element_size(uint64_t s) {
  structElementSize.push(s);
  return;
}

void InterpreterObserver::push_getelementptr_inx(uint64_t index) {
  getElementPtrIndexList.push_back(index);
  return;
}

int InterpreterObserver::actualValueToIntValue(int scope, int64_t vori) {
  switch (scope) {
    case CONSTANT:
      return vori;
    case LOCAL:
      return executionStack.top()[vori]->getIntValue();
    case GLOBAL:
      return globalSymbolTable[vori]->getIntValue();
    default:
      return -1;
  }
}

void InterpreterObserver::push_getelementptr_inx5(int scope01, int scope02, int
    scope03, int scope04, int scope05, int64_t vori01, int64_t vori02, int64_t
    vori03, int64_t vori04, int64_t vori05) {

  int value;
  if (scope01 != SCOPE_INVALID) {
    value = actualValueToIntValue(scope01, vori01);
    getElementPtrIndexList.push_back(value);

    if (scope02 != SCOPE_INVALID) {
      value = actualValueToIntValue(scope02, vori02);
      getElementPtrIndexList.push_back(value);

      if (scope03 != SCOPE_INVALID) {
	value = actualValueToIntValue(scope03, vori03);
        getElementPtrIndexList.push_back(value);

        if (scope04 != SCOPE_INVALID) {
	  value = actualValueToIntValue(scope04, vori04);
          getElementPtrIndexList.push_back(value);

          if (scope05 != SCOPE_INVALID) {
	    value = actualValueToIntValue(scope05, vori05);
            getElementPtrIndexList.push_back(value);
          }
        }
      }
    }
  }
  return;
}

void InterpreterObserver::push_getelementptr_inx2(int int_value) {
  int idx = int_value;
  getElementPtrIndexList.push_back(idx);
  return;
}

void InterpreterObserver::push_array_size(uint64_t size) {
  arraySize.push_back(size);
  return;
}

void InterpreterObserver::push_array_size5(int s1, int s2, int s3, int s4, int s5) {
  if (s1 != -1) {
    arraySize.push_back(s1);
    if (s2 != -1) {
      arraySize.push_back(s2);
      if (s3 != -1) {
        arraySize.push_back(s3);
        if (s4 != -1) {
          arraySize.push_back(s4);
          if (s5 != -1) {
            arraySize.push_back(s5);
          }
        }
      }
    }
  }
  return;
}

void InterpreterObserver::after_call(int retInx UNUSED, SCOPE retScope UNUSED, KIND retType, int64_t retValue) {

  if (!isReturn) {
    //int callerId = callerVarIndex.top();
    //pre_sync_call(callerId, line);

    // call is not interpreted
    safe_assert(!callerVarIndex.empty());

    // empty myStack and callArgs
    while (!myStack.empty()) {
      delete myStack.top();
      myStack.pop();
    }
    while (!callArgs.empty()) {
      delete callArgs.top();
      callArgs.pop();
    }

    IValue* reg = executionStack.top()[callerVarIndex.top()];

    // setting return value
    reg->setTypeValue(retType, retValue);
    reg->setValueOffset(0); // new
    reg->setShadow(0);
    callerVarIndex.pop();

    //post_sync_call(callerId, line);

    DEBUG_STDOUT(reg->toString());
  } else {
    safe_assert(callArgs.empty());
    safe_assert(myStack.empty());
  }

  isReturn = false;

  safe_assert(!recentBlock.empty());
  recentBlock.pop();
  return;
}

void InterpreterObserver::after_void_call() {

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
  return;
}

void InterpreterObserver::after_struct_call() {

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
    //IValue* structValue = (IValue*) malloc(returnStruct.size()*sizeof(IValue)); //
    IValue* structValue = new IValue[returnStruct.size()];

    for(unsigned i = 0; i < returnStruct.size(); i++) {
      KVALUE* value = returnStruct[i];
      IValue* iValue = new IValue(value->kind);
      iValue->setValue(value->value);
      iValue->setLength(0);
      structValue[i] = *iValue; 
    }
    returnStruct.clear();

    executionStack.top()[callerVarIndex.top()] = structValue;

    DEBUG_STDOUT(executionStack.top()[callerVarIndex.top()]->toString());

    callerVarIndex.pop();
  } 
  else {
    returnStruct.clear();
    safe_assert(callArgs.empty());
    safe_assert(myStack.empty());
  }

  isReturn = false;

  safe_assert(!recentBlock.empty());
  recentBlock.pop();
  return;
}

void InterpreterObserver::create_stack_frame(int size) {

  isReturn = false;

  std::vector<IValue*> frame (size);
  for (int i = 0; i < size; i++) {
    if (!callArgs.empty()) {
      frame[i] = callArgs.top();
      DEBUG_STDOUT("\t Argument " << i << ": " << frame[i]->toString());
      callArgs.pop();
    } 
    else {
      frame[i] = new IValue();
    }
  }
  safe_assert(callArgs.empty());
  executionStack.push(frame);
  return;
}

void InterpreterObserver::create_global_symbol_table(int size) {

  //pre_create_global_symbol_table();

  // instantiate copyShadow
  IValue::setCopyShadow(&copyShadow);

  // get log name
  int length, i;
  char* log;

  length = logName.size();
  log = new char[length+1];

  for (i = 0; i < length; i++) {
    log[length-i-1] = logName.top();
    logName.pop();
  }

  log[length] = '\0';

  // initialize logger
  google::InitGoogleLogging(log);
  DEBUG_LOG("Initialized logger");

  for (int i = 0; i < size; i++) {
    IValue* value = new IValue();
    globalSymbolTable.push_back(value);
  }

  //pre_analysis();

  //post_create_global_symbol_table();

  // free memory
  /*
  for(unsigned int i = 0; i < globalSymbolTable.size(); i++) {
    delete globalSymbolTable[i];
  }
  */
  delete[] log;
  return;
}

void InterpreterObserver::record_block_id(int id) {

  if (recentBlock.empty()) {
    recentBlock.push(id);
  } 
  else {
    recentBlock.pop();
    recentBlock.push(id);
  }
  return;
}

void InterpreterObserver::create_global_array(int valInx, uint64_t addr, uint32_t size, KIND type) {
  IValue *location = new IValue[size];
  collect_new.push_back(location);
  uint32_t i, elemSize;
  VALUE zero, value;

  // initialize the array
  elemSize = KIND_GetSize(type);
  zero.as_int = 0;
  for (i = 0; i < size; i++) {    
    location[i].setType(type);
    location[i].setValue(zero);
    location[i].setScope(REGISTER);
    location[i].setFirstByte(i*elemSize);
  } 

  value.as_ptr = (void*)addr;
  IValue *ptrLocation = new IValue(PTR_KIND, value, GLOBAL);
  ptrLocation->setSize(KIND_GetSize(type));
  ptrLocation->setLength(size);
  ptrLocation->setValueOffset((int64_t)location - value.as_int);

  release(globalSymbolTable[valInx]);
  globalSymbolTable[valInx] = ptrLocation;
  DEBUG_STDOUT("\tptr: " << ptrLocation->toString());
  return;
}

void InterpreterObserver::create_global(KVALUE* kvalue, KVALUE* initializer) {

  // allocate object
  IValue* location;
  location = new IValue(initializer->kind, initializer->value, GLOBAL); // GLOBAL?

  VALUE value;
  value.as_ptr = kvalue->value.as_ptr;
  IValue* ptrLocation = new IValue(PTR_KIND, value, GLOBAL);
  ptrLocation->setSize(KIND_GetSize(initializer->kind)); // put in constructor
  if (location->getType() != PTR_KIND) {
    ptrLocation->setLength(1);
  }
  ptrLocation->setValueOffset((int64_t)location - value.as_int);

  // store it in globalSymbolTable
  release(globalSymbolTable[kvalue->inx]);
  globalSymbolTable[kvalue->inx] = ptrLocation;
  DEBUG_STDOUT("\tloc: " << location->toString());
  DEBUG_STDOUT("\tptr: " << ptrLocation->toString());
  return;
}

void InterpreterObserver::call(IID iid UNUSED, bool nounwind UNUSED, KIND type, int inx) {

  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();

    DEBUG_STDOUT(", arg: " << KVALUE_ToString(value).c_str());

    IValue* argCopy;
    if (value->inx != -1) {
      IValue* arg = value->isGlobal ? globalSymbolTable[value->inx] :
        executionStack.top()[value->inx];
      safe_assert(arg);
      argCopy = new IValue();
      arg->copy(argCopy);
    } 
    else {
      // argument is a constant
      argCopy = new IValue(value->kind, value->value, LOCAL);
      argCopy->setLength(0); // uninitialized pointer
    }
    callArgs.push(argCopy);
  }

  if (type != VOID_KIND) {
    callerVarIndex.push(inx); 
  }

  IValue* callValue = executionStack.top()[inx];
  callValue->clear();
  callValue->setType(type);

  DEBUG_STDOUT(executionStack.top()[inx]->toString());

  // new recentBLock stack frame for the new call
  recentBlock.push(0);
  return;
}

void InterpreterObserver::call_sin(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type, int inx) {

  safe_assert(myStack.size() == 1);
  KVALUE *arg = myStack.top();
  myStack.pop();
  double argValue;
  VALUE value;
  //SCOPE argScope;
  //int64_t argValueOrIndex;

  // Get the operand value.
  if (arg->inx != -1) {
    IValue *iArg;

    if (arg->isGlobal) {
      iArg = globalSymbolTable[arg->inx];
      //argScope = GLOBAL;
    } 
    else {
      iArg = executionStack.top()[arg->inx];
      //argScope = LOCAL;
    }

    safe_assert(iArg);

    argValue = iArg->getFlpValue();
  } 
  else {
    //argScope = CONSTANT;
    argValue = arg->value.as_flp;
  }

  /*
  if (argScope == CONSTANT) {
    int64_t *ptr = (int64_t*)&argValue;
    argValueOrIndex = *ptr;
  } 
  else {
    argValueOrIndex = arg->inx;
  }
  */

  //pre_call_sin(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);

  value.as_flp = sin(argValue); 
  IValue *returnValue = new IValue(type, value);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = returnValue;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  //post_call_sin(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);
  return;
}

void InterpreterObserver::call_acos(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type, int inx) {

  safe_assert(myStack.size() == 1);
  KVALUE *arg = myStack.top();
  myStack.pop();
  double argValue;
  VALUE value;
  //SCOPE argScope;
  //int64_t argValueOrIndex;

  // Get the operand value.
  if (arg->inx != -1) {
    IValue *iArg;

    if (arg->isGlobal) {
      iArg = globalSymbolTable[arg->inx];
      //argScope = GLOBAL;
    } 
    else {
      iArg = executionStack.top()[arg->inx];
      //argScope = LOCAL;
    }

    safe_assert(iArg);

    argValue = iArg->getFlpValue();
  } 
  else {
    //argScope = CONSTANT;
    argValue = arg->value.as_flp;
  }

  /*
  if (argScope == CONSTANT) {
    int64_t *ptr = (int64_t*)&argValue;
    argValueOrIndex = *ptr;
  } 
  else {
    argValueOrIndex = arg->inx;
  }
  */

  //pre_call_acos(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);

  value.as_flp = acos(argValue); 
  IValue *returnValue = new IValue(type, value);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = returnValue;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  //post_call_acos(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);
  return;
}

void InterpreterObserver::call_sqrt(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type, int inx) {

  safe_assert(myStack.size() == 1);
  KVALUE *arg = myStack.top();
  myStack.pop();
  double argValue;
  VALUE value;
  //SCOPE argScope;
  //int64_t argValueOrIndex;

  // Get the operand value.
  if (arg->inx != -1) {
    IValue *iArg;

    if (arg->isGlobal) {
      iArg = globalSymbolTable[arg->inx];
      //argScope = GLOBAL;
    } 
    else {
      iArg = executionStack.top()[arg->inx];
      //argScope = LOCAL;
    }

    safe_assert(iArg);

    argValue = iArg->getFlpValue();
  } 
  else {
    //argScope = CONSTANT;
    argValue = arg->value.as_flp;
  }

  /*
  if (argScope == CONSTANT) {
    int64_t *ptr = (int64_t*)&argValue;
    argValueOrIndex = *ptr;
  } 
  else {
    argValueOrIndex = arg->inx;
  }
  */

  //pre_call_sqrt(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);

  value.as_flp = sqrt(argValue); 
  IValue *returnValue = new IValue(type, value);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = returnValue;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  //post_call_sqrt(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);
  return;
}

void InterpreterObserver::call_fabs(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type, int inx) {

  safe_assert(myStack.size() == 1);
  KVALUE *arg = myStack.top();
  myStack.pop();
  double argValue;
  VALUE value;
  //SCOPE argScope;
  //int64_t argValueOrIndex;

  // Get the operand value.
  if (arg->inx != -1) {
    IValue *iArg;

    if (arg->isGlobal) {
      iArg = globalSymbolTable[arg->inx];
      //argScope = GLOBAL;
    } 
    else {
      iArg = executionStack.top()[arg->inx];
      //argScope = LOCAL;
    }

    safe_assert(iArg);

    argValue = iArg->getFlpValue();
  } 
  else {
    //argScope = CONSTANT;
    argValue = arg->value.as_flp;
  }

  /*
  if (argScope == CONSTANT) {
    int64_t *ptr = (int64_t*)&argValue;
    argValueOrIndex = *ptr;
  } 
  else {
    argValueOrIndex = arg->inx;
  }
  */

  //pre_call_fabs(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);

  value.as_flp = fabs(argValue); 
  IValue *returnValue = new IValue(type, value);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = returnValue;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  //post_call_fabs(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);
  return;
}

void InterpreterObserver::call_cos(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type, int inx) {

  safe_assert(myStack.size() == 1);
  KVALUE *arg = myStack.top();
  myStack.pop();
  double argValue;
  VALUE value;
  //SCOPE argScope;
  //int64_t argValueOrIndex;

  // Get the operand value.
  if (arg->inx != -1) {
    IValue *iArg;

    if (arg->isGlobal) {
      iArg = globalSymbolTable[arg->inx];
      //argScope = GLOBAL;
    } 
    else {
      iArg = executionStack.top()[arg->inx];
      //argScope = LOCAL;
    }

    safe_assert(iArg);

    argValue = iArg->getFlpValue();
  } 
  else {
    //argScope = CONSTANT;
    argValue = arg->value.as_flp;
  }

  /*
  if (argScope == CONSTANT) {
    int64_t *ptr = (int64_t*)&argValue;
    argValueOrIndex = *ptr;
  } 
  else {
    argValueOrIndex = arg->inx;
  }
  */

  //pre_call_cos(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);

  value.as_flp = cos(argValue); 
  IValue *returnValue = new IValue(type, value);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = returnValue;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  //post_call_cos(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);
  return;
}


void InterpreterObserver::call_log(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type, int inx) {

  safe_assert(myStack.size() == 1);
  KVALUE *arg = myStack.top();
  myStack.pop();
  double argValue;
  VALUE value;
  //SCOPE argScope;
  //int64_t argValueOrIndex;

  // Get the operand value.
  if (arg->inx != -1) {
    IValue *iArg;

    if (arg->isGlobal) {
      iArg = globalSymbolTable[arg->inx];
      //argScope = GLOBAL;
    } 
    else {
      iArg = executionStack.top()[arg->inx];
      //argScope = LOCAL;
    }

    safe_assert(iArg);

    argValue = iArg->getFlpValue();
  } 
  else {
    //argScope = CONSTANT;
    argValue = arg->value.as_flp;
  }

  /*
  if (argScope == CONSTANT) {
    int64_t *ptr = (int64_t*)&argValue;
    argValueOrIndex = *ptr;
  } 
  else {
    argValueOrIndex = arg->inx;
  }
  */

  //pre_call_log(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);

  value.as_flp = log(argValue); 
  IValue *returnValue = new IValue(type, value);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = returnValue;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  //post_call_log(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);
  return;
}

void InterpreterObserver::call_floor(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type, int inx) {

  safe_assert(myStack.size() == 1);
  KVALUE *arg = myStack.top();
  myStack.pop();
  double argValue;
  VALUE value;
  //SCOPE argScope;
  //int64_t argValueOrIndex;

  // Get the operand value.
  if (arg->inx != -1) {
    IValue *iArg;

    if (arg->isGlobal) {
      iArg = globalSymbolTable[arg->inx];
      //argScope = GLOBAL;
    } 
    else {
      iArg = executionStack.top()[arg->inx];
      //argScope = LOCAL;
    }

    safe_assert(iArg);

    argValue = iArg->getFlpValue();
  } 
  else {
    //argScope = CONSTANT;
    argValue = arg->value.as_flp;
  }

  /*
  if (argScope == CONSTANT) {
    int64_t *ptr = (int64_t*)&argValue;
    argValueOrIndex = *ptr;
  } 
  else {
    argValueOrIndex = arg->inx;
  }
  */

  //pre_call_floor(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);

  value.as_flp = floor(argValue); 
  IValue *returnValue = new IValue(type, value);

  release(executionStack.top()[inx]);
  executionStack.top()[inx] = returnValue;

  DEBUG_STDOUT(executionStack.top()[inx]->toString());
  //post_call_floor(iid, nounwind, pc, type, inx, argScope, argValueOrIndex);
  return;
}

void InterpreterObserver::call_malloc(IID iid UNUSED, bool nounwind UNUSED, KIND type, 
				      int size, int inx, uint64_t mallocAddress) {

  // retrieving original number of bytes
  KVALUE* argValue = myStack.top();
  myStack.pop();
  assert(myStack.size() == 0);
  
  if (type != STRUCT_KIND) {
    // allocating space
    int numObjects = argValue->value.as_int*8 / size;
    IValue *addr = new IValue[numObjects];
    collect_new.push_back(addr);

    // creating pointer object
    VALUE returnValue;
    returnValue.as_ptr = (void*)mallocAddress;
    IValue* newPointer = new IValue(PTR_KIND, returnValue, size/8, 0, 0, numObjects);
    newPointer->setValueOffset((int64_t)addr - (int64_t)returnValue.as_ptr);
    release(executionStack.top()[inx]);
    executionStack.top()[inx] = newPointer;

    // creating locations
    unsigned currOffset = 0;
    for(int i = 0; i < numObjects; i++) {
      // creating object
      VALUE iValue;
      // TODO: check if we need uninitialized value
      addr[i].setType(type);
      addr[i].setValue(iValue);
      addr[i].setFirstByte(currOffset);
      addr[i].setValueOffset(newPointer->getValueOffset());
      
      // updating offset
      currOffset += (size/8);
    }

    DEBUG_STDOUT(executionStack.top()[inx]->toString());
  } else {

    // allocating space
    unsigned numStructs = ceil(argValue->value.as_int*8.0 / size);
    unsigned fields = structType.size();

    IValue *ptrToStructVar = new IValue[numStructs*fields];
    collect_new.push_back(ptrToStructVar);

    DEBUG_STDOUT("\nTotal size of malloc in bits: " << argValue->value.as_int*8);
    DEBUG_STDOUT("Size: " << size);
    DEBUG_STDOUT("Num Structs: " << numStructs);
    DEBUG_STDOUT("Number of fields: " << fields);

    KIND fieldTypes[fields];
    for(unsigned i = 0; i < fields; i++) {
      fieldTypes[i] = structType[i];
    }
    structType.clear();

    unsigned length = 0;
    unsigned firstByte = 0;
    for(unsigned i = 0; i < numStructs; i++) {
      for (unsigned j = 0; j < fields; j++) {
        KIND type = fieldTypes[j];

	ptrToStructVar[length].setType(type);
	ptrToStructVar[length].setFirstByte(firstByte);
        firstByte = firstByte + KIND_GetSize(type);
        DEBUG_STDOUT("Created a field of struct: " << length);
        DEBUG_STDOUT(ptrToStructVar[length].toString());
        length++;
      }
    }

    VALUE structPtrVal;
    structPtrVal.as_ptr = (void*)mallocAddress;
    IValue* structPtrVar = new IValue(PTR_KIND, structPtrVal);
    structPtrVar->setValueOffset((int64_t)ptrToStructVar - (int64_t)mallocAddress);  ////////////
    structPtrVar->setSize(KIND_GetSize(ptrToStructVar[0].getType()));
    structPtrVar->setLength(length);

    release(executionStack.top()[inx]);
    executionStack.top()[inx] = structPtrVar;
    DEBUG_STDOUT(structPtrVar->toString());
  }
  return;
}

void InterpreterObserver::vaarg() {
  cerr << "[InterpreterObserver::vaarg] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::landingpad() {
  cerr << "[InterpreterObserver::landingpad] => Unimplemented\n";
  safe_assert(false);
}

void InterpreterObserver::printCurrentFrame() {
  DEBUG_STDOUT("Print current frame.");
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
  double cValueLD;

  switch (type) {
    case PTR_KIND:
      // TODO: we use int64_t to represent a void* here
      // might not work on 32 bit machine
      cValueVoid = *((int64_t*) concrete->value.as_ptr);

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
        cValueInt8Arr = (int8_t*) calloc(8, sizeof(int8_t));
        cValueInt8Arr[0] = cValueInt32;
        syncValue.as_int = *((int64_t*) cValueInt8Arr);
        iValue->setValue(syncValue);
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
        cValueInt32Arr = (int32_t*) calloc(2, sizeof(int32_t));
        cValueInt32Arr[0] = cValueInt32; 
        syncValue.as_int = *((int32_t*) cValueInt32Arr);
        iValue->setValue(syncValue);
      }
      break;
    case INT32_KIND: 
      cValueInt32 = *((int32_t*) concrete->value.as_ptr);
      sync = (((int32_t) iValue->getIntValue()) != cValueInt32);
      if (sync) {
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
      if (isnan((float)iValue->getValue().as_flp) && isnan(cValueFloat)) {
        sync = false;
      }
      else {
        sync = ((float)iValue->getValue().as_flp != cValueFloat);
      }
      if (sync) {
        syncValue.as_flp = cValueFloat;
        iValue->setValue(syncValue);
      }
      break;
    case FLP64_KIND:
      cValueDouble = *((double*) concrete->value.as_ptr);
      if (isnan((double)iValue->getValue().as_flp) && isnan(cValueDouble)) {
        sync = false;
      }
      else {
        sync = ((double)iValue->getValue().as_flp != cValueDouble);
      }
      if (sync) {
        syncValue.as_flp = cValueDouble;
        iValue->setValue(syncValue);
      }
      break;
    case FLP80X86_KIND:
      cValueLD = *((long double*) concrete->value.as_ptr);
      if (isnan((double)iValue->getValue().as_flp) && isnan(cValueLD)) {
        sync = false;
      }
      else {
        sync = ((double)iValue->getValue().as_flp != cValueLD);
      }
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
    DEBUG_STDOUT("\t SYNCING AT LOAD DUE TO MISMATCH");
    DEBUG_STDOUT("\t " << iValue->toString());
  }

  return sync;
}

bool InterpreterObserver::syncLoad(IValue* iValue, uint64_t concreteAddr, KIND type) { 
  bool sync = false;
  VALUE syncValue;
  int cValueInt32;
  int64_t cValueInt64;
  float cValueFloat;
  double cValueDouble;
  double cValueLD;

  void *concreteValue;

  concreteValue = (void *) concreteAddr;

  switch (type) {
    case PTR_KIND:
      // TODO: we use int64_t to represent a void* here
      // might not work on 32 bit machine
      cValueInt64 = *((int64_t*) concreteValue);

      sync = (iValue->getValue().as_int + iValue->getOffset() != cValueInt64);
      if (sync) {
        syncValue.as_int = cValueInt64;
        iValue->setValue(syncValue);
      }
      break;
    case INT1_KIND: 
    case INT8_KIND: 
      cValueInt32 = *((int8_t*) concreteValue);

      sync = (((int8_t) iValue->getIntValue()) != cValueInt32);
      if (sync) {
        syncValue.as_int = cValueInt32;
        iValue->setValue(syncValue);
      }
      break;
    case INT16_KIND: 
      {
        cValueInt32 = *((int16_t*) concreteValue);
        sync = (((int16_t) iValue->getIntValue()) != cValueInt32);
        if (sync) {
          syncValue.as_int = cValueInt32;
          iValue->setValue(syncValue);
        }
        break;
      }
    case INT24_KIND:
      cValueInt32 = *((int32_t*) concreteValue);
      cValueInt32 = cValueInt32 & 0x00FFFFFF;
      sync = (((int32_t) iValue->getIntValue()) != cValueInt32);
      if (sync) {
        syncValue.as_int = cValueInt32;
        iValue->setValue(syncValue);
      }
      break;
    case INT32_KIND: 
      cValueInt32 = *((int32_t*) concreteValue);
      sync = (((int32_t) iValue->getIntValue()) != cValueInt32);
      if (sync) {
        syncValue.as_int = cValueInt32;
        iValue->setValue(syncValue);
      }
      break;
    case INT64_KIND:
      cValueInt64 = *((int64_t*) concreteValue);
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
      cValueFloat = *((float*) concreteValue);
      if (isnan((float)iValue->getValue().as_flp) && isnan(cValueFloat)) {
        sync = false;
      } else {
        sync = ((float)iValue->getValue().as_flp != cValueFloat);
      }
      if (sync) {
        syncValue.as_flp = cValueFloat;
        iValue->setValue(syncValue);
      }
      break;
    case FLP64_KIND:
      cValueDouble = *((double*) concreteValue);
      if (isnan((double)iValue->getValue().as_flp) && isnan(cValueDouble)) {
        sync = false;
      } else {
        sync = ((double)iValue->getValue().as_flp != cValueDouble);
      }
      if (sync) {
        syncValue.as_flp = cValueDouble;
        iValue->setValue(syncValue);
      }
      break;
    case FLP80X86_KIND:
      {
        cValueLD = *((long double*) concreteValue);
        if (isnan((double)iValue->getValue().as_flp) && isnan(cValueLD)) {
          sync = false;
        } else {
          sync = ((double)iValue->getValue().as_flp != cValueLD);
        }
        if (sync) {
          syncValue.as_flp = cValueLD;
          iValue->setValue(syncValue);
        }
        break;
      }
    default: 
      cout << "Should not reach here!" << endl;
      safe_assert(false);
      break;
  }

  if (sync) {
    DEBUG_STDOUT("\t SYNCING AT LOAD DUE TO MISMATCH");
    DEBUG_STDOUT("\t " << iValue->toString());
  }

  return sync;
}


void InterpreterObserver::pre_allocax(IID iid UNUSED, KIND type UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* actualAddress UNUSED) {}

void InterpreterObserver::post_allocax(IID iid UNUSED, KIND type UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* actualAddress UNUSED) {}

void InterpreterObserver::pre_load(IID iid UNUSED, KIND type UNUSED, SCOPE opScope UNUSED, int opInx UNUSED, uint64_t opAddr UNUSED, bool 
				   loadGlobal UNUSED, int loadInx UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {}
 
void InterpreterObserver::post_load(IID iid UNUSED, KIND type UNUSED, SCOPE opScope UNUSED, int opInx UNUSED, uint64_t opAddr UNUSED, bool 
				   loadGlobal UNUSED, int loadInx UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {}

void InterpreterObserver::pre_load_struct(IID iid UNUSED, KIND kind UNUSED, KVALUE* op UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {}

void InterpreterObserver::post_load_struct(IID iid UNUSED, KIND kind UNUSED, KVALUE* op UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {}

void InterpreterObserver::pre_store(int destInx UNUSED, SCOPE destScope UNUSED, KIND srcKind UNUSED, SCOPE srcScope UNUSED, int srcInx UNUSED, int64_t srcValue UNUSED, 
				    int file UNUSED, int line UNUSED, int inx UNUSED) {}

void InterpreterObserver::post_store(int destInx UNUSED, SCOPE destScope UNUSED, KIND srcKind UNUSED, SCOPE srcScope UNUSED, int srcInx UNUSED, int64_t srcValue UNUSED, 
				     int file UNUSED, int line UNUSED, int inx UNUSED) {}

void InterpreterObserver::pre_fadd(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::post_fadd(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::pre_fsub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::post_fsub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::pre_fmul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::post_fmul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::pre_fdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::post_fdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {}

void InterpreterObserver::pre_create_global_symbol_table() {}

void InterpreterObserver::post_create_global_symbol_table() {}

void InterpreterObserver::post_analysis() {}
