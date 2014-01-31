/**
 * @file OutOfBoundAnalysis.cpp
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

// Author: Cuong Nguyen

#include "OutOfBoundAnalysis.h"

void OutOfBoundAnalysis::getelementptr(IID iid UNUSED, bool inbound UNUSED, KVALUE* base,
    KVALUE* offset, KIND type UNUSED, uint64_t size, int line, int inx) {

  IValue* basePtrLocation = executionStack.top()[base->inx];
  IValue* ptrLocation;

  bool propagate = isOutOfBound(basePtrLocation);

  if (basePtrLocation->isInitialized()) {
    int offsetValue = offset->inx != -1 ?
      executionStack.top()[offset->inx]->getValue().as_int :
      offset->value.as_int;

    unsigned newOffset = (offsetValue * (size/8)) +
      basePtrLocation->getOffset();

    unsigned index = findIndex((IValue*) basePtrLocation->getValue().as_ptr,
        newOffset, basePtrLocation->getLength()); 
    ptrLocation = new IValue(PTR_KIND, basePtrLocation->getValue(), size/8,
        newOffset, index, basePtrLocation->getLength());

    if (propagate && isOutOfBound(ptrLocation)) {
      int baseLine = *((int*) basePtrLocation->getShadow());
      cout << line << " : [BUG WARNING] " << endl; 
      cout << "\t Potentially invalid memory pointer." << endl;
      cout << "\t Propagated from " << baseLine << "." << endl; 
      cout << endl;
    } else if (propagate && !isOutOfBound(ptrLocation)) {
      int baseLine = *((int*) basePtrLocation->getShadow());
      cout << line << " : [BUG SUPPRESSED] " << endl; 
      cout << "\t Potentially invalid memory pointer suppressed." << endl;
      cout << "\t Propagated from " << baseLine << "." << endl; 
      cout << endl;
    } else if (isOutOfBound(ptrLocation)) {
      cout << line << " : [BUG WARNING] " << endl; 
      cout << "\t Potentially invalid memory pointer." << endl;
      cout << endl;
    }
  } else {
    ptrLocation = new IValue(PTR_KIND, basePtrLocation->getValue(), size/8, 0,
        0, 0);
  }

  int* shadow = (int*) malloc(sizeof(int));
  shadow[0] = line;
  ptrLocation->setShadow((void*)shadow);
  ptrLocation->setLineNumber(line);

  executionStack.top()[inx] = ptrLocation;

  return;
}

void OutOfBoundAnalysis::getelementptr_array(IID iid UNUSED, bool inbound UNUSED, KVALUE* op, KIND kind UNUSED, int elementSize UNUSED, int inx) {

  IValue* ptrArray = executionStack.top()[op->inx];
  IValue* array = static_cast<IValue*>(ptrArray->getValue().as_ptr);

  getElementPtrIndexList.pop();

  unsigned index = 1;
  arraySize.pop();
  while (!arraySize.empty()) {
    index = index * arraySize.front();
    arraySize.pop();
  }
  index = getElementPtrIndexList.front()*index;
  getElementPtrIndexList.pop();
  safe_assert(getElementPtrIndexList.empty());

  index = ptrArray->getIndex() + index;

  IValue* arrayElem = array + index;
  IValue* arrayElemPtr = new IValue(PTR_KIND, ptrArray->getValue());
  arrayElemPtr->setIndex(index);
  arrayElemPtr->setLength(ptrArray->getLength());

  if (index < ptrArray->getLength()) {
    arrayElemPtr->setSize(KIND_GetSize(arrayElem[0].getType()));
    arrayElemPtr->setOffset(arrayElem[0].getFirstByte());
  } 

  if (isOutOfBound(arrayElemPtr)) {
    cout << "[BUG WARNING] Potentially invalid memory pointer!\n" << endl;
  }

  executionStack.top()[inx] = arrayElemPtr;
}

void OutOfBoundAnalysis::getelementptr_struct(IID iid UNUSED, bool inbound UNUSED, KVALUE* op, KIND kind UNUSED, KIND arrayKind UNUSED, int inx) {
  IValue* structPtr = executionStack.top()[op->inx];
  IValue* structBase = static_cast<IValue*>(structPtr->getValue().as_ptr);

  unsigned index;
  getElementPtrIndexList.pop();
  index = getElementPtrIndexList.front();
  getElementPtrIndexList.pop();
  safe_assert(getElementPtrIndexList.empty());
  index = structPtr->getIndex() + index;

  IValue* structElem = structBase + index;
  IValue* structElemPtr = new IValue(PTR_KIND, structPtr->getValue());
  structElemPtr->setIndex(index);
  structElemPtr->setLength(structPtr->getLength());

  if (index < structPtr->getLength()) {
    structElemPtr->setSize(KIND_GetSize(structElem->getType()));
    structElemPtr->setOffset(structElem->getFirstByte());
  }

  if (isOutOfBound(structElemPtr)) {
    cout << "[BUG WARNING] Potentially invalid memory pointer!\n" << endl;
  }

  executionStack.top()[inx] = structElemPtr;

}

void OutOfBoundAnalysis::load(IID iid UNUSED, KIND type, KVALUE* src, int file UNUSED, int line, int inx) {
  IValue* srcPtrLocation = src->isGlobal ? globalSymbolTable[src->inx] :
    executionStack.top()[src->inx];

  IValue *destLocation = new IValue();

  if (isOutOfBound(srcPtrLocation)) {
    int baseLine = *((int*) srcPtrLocation->getShadow());
    cout << line << " : [BUG WARNING]" << endl; 
    cout << "\tPotentially invalid memory load." << endl; 
    cout << "\tPropagated from " << baseLine << "." << endl;
  }

  if (srcPtrLocation->isInitialized() && !isOutOfBound(srcPtrLocation)) {
    IValue *srcLocation = NULL;
    unsigned srcOffset = srcPtrLocation->getOffset();
    int internalOffset = 0;

    // retrieving source
    IValue* values = (IValue*)srcPtrLocation->getValue().as_ptr;
    unsigned valueIndex = srcPtrLocation->getIndex();
    unsigned currOffset = values[valueIndex].getFirstByte();

    srcLocation = &values[valueIndex];

    internalOffset = srcOffset - currOffset;

    VALUE value = srcPtrLocation->readValue(internalOffset, type);

    srcLocation->copy(destLocation);
    destLocation->setSize(KIND_GetSize(type));
    destLocation->setValue(value);
    destLocation->setType(type);

    // sync load
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

  int* shadow = (int*) malloc(sizeof(int));
//  shadow[0] = line;
  shadow[0] = srcPtrLocation->getLineNumber();
  destLocation->setShadow((void*) shadow);
  destLocation->setLineNumber(line);

  executionStack.top()[inx] = destLocation;

  return;
}

void OutOfBoundAnalysis::store(IID iid UNUSED, KVALUE* dest, KVALUE* src, int file UNUSED, int line, int inx UNUSED) {
  IValue* destPtrLocation = dest->isGlobal ? globalSymbolTable[dest->inx] :
    executionStack.top()[dest->inx];


  if (!isOutOfBound(destPtrLocation)) {

    unsigned destPtrOffset = destPtrLocation->getOffset();
    IValue *srcLocation = src->iid == 0 ? new IValue(src->kind, src->value) :
      executionStack.top()[src->inx];
    int internalOffset = 0;

    if (src->iid != 0) {
      destPtrLocation->setShadow(srcLocation->getShadow());
      destPtrLocation->setLineNumber(srcLocation->getLineNumber());
    }

    // retrieve actual destination
    IValue* values = (IValue*)destPtrLocation->getValue().as_ptr;
    unsigned valueIndex = destPtrLocation->getIndex();
    unsigned currOffset = values[valueIndex].getFirstByte();
    internalOffset = destPtrOffset - currOffset;

    // writing src into dest
    destPtrLocation->writeValue(internalOffset, destPtrLocation->getSize(), srcLocation);

    // just read again to check store
    IValue* writtenValue = new IValue(srcLocation->getType(),
        destPtrLocation->readValue(internalOffset, src->kind)); 

    if (!checkStore(writtenValue, src)) { // destLocation
      cerr << "\tKVALUE: " << KVALUE_ToString(src) << endl;
      cerr << "\tMismatched values found in Store" << endl;
      abort();
    }

  } else {
    cout << line << " : [BUG WARNING] Potentially invalid memory store!\n" << endl; 
  }

  return;
}

bool OutOfBoundAnalysis::isOutOfBound(IValue* pointer) {
  if (pointer->getType() == PTR_KIND) {
    if (pointer->getIndex() >= pointer->getLength()) {
      return true;
    } else if (pointer->getIndex() == pointer->getLength() -1) {
      IValue* array = static_cast<IValue*>(pointer->getValue().as_ptr);
      IValue* arrayLem = array + pointer->getIndex();
      unsigned internalOffset = pointer->getOffset() - arrayLem->getFirstByte();
      int address = internalOffset + pointer->getSize();
      if (address > KIND_GetSize(arrayLem->getType())) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}
