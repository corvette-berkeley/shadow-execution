/**
 * @file IValue.cpp
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

#include "IValue.h"

//int IValue::counterNew = 0;
//int IValue::counterDelete = 0;

void (*IValue::copyShadow)(IValue*,IValue*) = NULL;

string IValue::toString() {
  std::stringstream s;

  switch(scope) {
    case GLOBAL:
      s << "GLOBAL   => ";
      break;
    case LOCAL:
      s << "LOCAL    => ";
      break;
    case REGISTER:
      s << "REGISTER => ";
      break;
    default:
      DEBUG_STDERR("IValue does not have valid scope."); 
      safe_assert(false);
  }

  switch(type) {
    case PTR_KIND:
      s << "[PTR: " << value.as_ptr << "]";
      break;
    case ARRAY_KIND:
      s << "[ARRAY: " << value.as_ptr << "]";
      break;
    case STRUCT_KIND:
      s << "[STRUCT: " << value.as_ptr << "]";
      break;
    case INT1_KIND:
      s << "[INT1: " << value.as_int << "] ";
      s << ", bitOffset: " << bitOffset;
      break;
    case INT8_KIND:
      s << "[INT8: " << value.as_int << "]";
      break;
    case INT16_KIND:
      s << "[INT16: " << value.as_int << "]";
      break;
    case INT24_KIND:
      s << "[INT24: " << value.as_int << "]";
      break;
    case INT32_KIND:
      s << "[INT32: " << value.as_int << "]";
      break;
    case INT64_KIND:
      s << "[INT64: " << value.as_int << "]";
      break;
    case FLP32_KIND:
      s << "[FLP32: " << (float)value.as_flp << "]";
      break;
    case FLP64_KIND:
      s << "[FLP64: " << (double) value.as_flp << "]";
      break;
    case FLP80X86_KIND:
      s << "[FLP80X86: " << (double) value.as_flp << "]";
      break;
    case VOID_KIND:
      s << "[VOID]";
      break;
    default: 
      DEBUG_STDERR("IValue type is not supported.");
      safe_assert(false);
      break;
  }

  s << ", Size: " << size;
  s << ", Offset: " << offset; 
  s << ", BitOffset: " << bitOffset;
  s << ", Index: " << index;
  s << ", Line: " << lineNumber;
  s << ", File: " << fileNumber;
  s << ", FirstByte: " << firstByte;
  s << ", Length: " << length;
  s << ", Initialized: " << isInitialized();
  s << ", ValueOffset: " << valueOffset;

  return s.str();

}

void IValue::copy(IValue *dest) {
  // note: we do never overwrite the field firstByte
  dest->setType(type);
  dest->setValue(value);
  dest->setSize(size);
  dest->setOffset(offset);
  dest->setBitOffset(bitOffset);
  dest->setIndex(index);
  dest->setLength(length);
  dest->setValueOffset(valueOffset);
  if (copyShadow != NULL) {
    copyShadow(this, dest);
  }
  // source info?
  dest->setLineNumber(lineNumber);
  dest->setFileNumber(fileNumber);
  return;
}

void IValue::copyFrom(KVALUE* kValue) {
  setType(kValue->kind);
  setValue(kValue->value);
}

VALUE IValue::readValue(int offset, KIND type) {

  int byte;
  VALUE value;
  IValue* valueArray;

  DEBUG_STDOUT("\t" << " Reading from IValue object: " << toString());
  DEBUG_STDOUT("\t" << " Reading for type: " << KIND_ToString(type)); 

  valueArray = static_cast<IValue*>(getIPtrValue());
  byte = KIND_GetSize(type);

  if (offset == 0 && KIND_GetSize(valueArray[index].getType()) == byte) {

    //
    // trivial reading case
    //
    
    DEBUG_STDOUT("\t" << "Trivial reading."); 
    value = valueArray[index].getValue();

  } else {

    //
    // off the shelf reading case
    //
    
    DEBUG_STDOUT("\t" << "Off the shelf reading."); 

    unsigned nextIndex;
    int totalByte, tocInx, trcInx;
    uint8_t *totalContent, *truncContent;
    
    nextIndex = index;
    totalByte = 0;

    // TODO: review the condition nextIndex < length
    while (totalByte < offset + byte && nextIndex < length) {
      IValue value; 
      
      value = valueArray[nextIndex];
      totalByte += KIND_GetSize(value.getType());
      nextIndex++;
    }

    //
    // totalContent stores the accumulative content from IValue at index 
    // to IValue at nextIndex-1
    //
    totalContent = (uint8_t*) malloc(totalByte*sizeof(uint8_t));
    tocInx = 0;

    for (unsigned i = index; i < nextIndex; i++) {
      IValue value;
      KIND type;
      int size;
      VALUE valValue;
      uint8_t* valueContent;

      value = valueArray[i];
      type = value.getType();
      size = KIND_GetSize(type);
      valValue = value.getValue();
      valueContent = (uint8_t*) &valValue;

      for (int j = 0; j < size; j++) {
        totalContent[tocInx] = valueContent[j];
        tocInx++;
      }
    }

    //
    // truncate content from total content
    //
    truncContent = (uint8_t*) calloc(8, sizeof(uint8_t)); // TODO: magic number 8 is 64/8
    trcInx = 0;

    for (int i = offset; i < offset + byte; i++) {
      truncContent[trcInx] = totalContent[i];
      trcInx++;
    }

    //
    // cast truncate content array to an actual value 
    //
    switch(type) {
      case FLP32_KIND: 
        {
          float* truncValue = (float*) truncContent;
          value.as_flp = *truncValue;
          break;
        }
      case FLP64_KIND: 
        {
          double* truncValue = (double*) truncContent;
          value.as_flp = *truncValue;
          break;
        }
      default: 
        {
          int64_t* truncValue = (int64_t*) truncContent;
          value.as_int = *truncValue;
          break;
        }
    }

  }

  return value;
}

int IValue::setValue(int offset, int byte, uint8_t* content) {
  uint8_t* valueBytes; 
  int maxOffset;
  
  valueBytes = (uint8_t*)&value;
  maxOffset = KIND_GetSize(type)-1;

  if (byte > maxOffset - offset +1) {
    byte = maxOffset - offset + 1;
  }

  for (int i = 0; i < byte; i++) {
    valueBytes[i+offset] = content[i];
  }

  return byte;
} 

bool IValue::writeValue(int offset, int byte, IValue* src) {
  int64_t valueOffset, length, bitOffset, newOffset;
  IValue* valueArray; 

  DEBUG_STDOUT("\tWriting value");
  DEBUG_STDOUT("\tInternal offset: " << offset);
  DEBUG_STDOUT("\tNumber of bytes to write: " << byte);
  DEBUG_STDOUT("\tGet value from: " << src->toString());
  
  valueArray = static_cast<IValue*>(getIPtrValue());
  valueOffset = src->getValueOffset();
  length = src->getLength();
  bitOffset = src->getBitOffset();
  newOffset = src->getOffset();

  if (offset == 0 && KIND_GetSize(valueArray[index].getType()) == byte) {

    //
    // trivial writing case
    //
    
    DEBUG_STDOUT("\t" << "Trivial writing."); 
    src->copy(&valueArray[index]);

    return true;

  } else {

    //
    // write off the shelf
    //

    DEBUG_STDOUT("\t" << "Off the shelf writing."); 

    VALUE srcValue; 
    uint8_t *srcContent, *content;
    int currentIndex, byteWrittens, oldByteWrittens;
    
    srcValue = src->getValue();

    //
    // get content from source value
    //
    srcContent = (uint8_t*)(&srcValue);
    content = (uint8_t*) malloc(byte*sizeof(uint8_t*));
    for (int i = 0; i < byte; i++) {
      content[i] = srcContent[i];
    }

    //
    // write the content to this value array
    //
    currentIndex = index;
    byteWrittens = 0;
    oldByteWrittens = 0;

    while (byteWrittens < byte) {
      IValue *currentValue; 
      
      currentValue = &valueArray[currentIndex];

      DEBUG_STDOUT("\t" << "=== Ivalue: " << currentValue->toString());
      DEBUG_STDOUT("\t" << "=== current content: " << (int64_t) *content);
      DEBUG_STDOUT("\t" << "=== current value: " << currentValue->getValue().as_int );

      byteWrittens += currentValue->setValue(offset, byte - byteWrittens, content);
      currentValue->setLength(length);
      currentValue->setValueOffset(valueOffset);
      currentValue->setBitOffset(bitOffset);
      currentValue->setOffset(newOffset);

      DEBUG_STDOUT("\t" << "=== byteWrittens: " << byteWrittens);
      DEBUG_STDOUT("\t" << "=== current value after: " << currentValue->getValue().as_int );
      DEBUG_STDOUT("\t" << "=== Ivalue after: " << currentValue->toString());

      content += byteWrittens - oldByteWrittens;
      newOffset = newOffset - byteWrittens;
      oldByteWrittens = byteWrittens;
      currentIndex++;
      offset = 0;
    }

    return false;
  }
}

bool IValue::isIValue(KIND t) {
  return type == t;
}

int64_t IValue::getIntValue() {
  int32_t v32;
  int32_t *v32Ptr;
  int64_t v64;
  int64_t *v64Ptr;

  //
  // truncating 64-bit integer value to 32-bit integer value
  //
  v64 = value.as_int;
  v64Ptr = &v64;
  v32Ptr = (int32_t*) v64Ptr;
  v32 = *v32Ptr;

  //
  // returning integer value depending on type
  //
  switch (type) {
    case INT1_KIND:
      return v32 & 0x1;;
    case INT8_KIND:
      return v32 & 0x000000FF;
    case INT16_KIND:
      return v32 & 0x0000FFFF;
    case INT24_KIND:
      return v32 & 0x00FFFFFF; 
    case INT32_KIND:
      return v32;
    case INT64_KIND:
      return v64;
    case INT80_KIND:
      DEBUG_STDERR("Unsupported type INT80.");
      safe_assert(false);
    default:
      return v64;
  }

}

uint64_t IValue::getUIntValue() {
  int64_t v64 = getIntValue();

  //
  // returning integer value depending on type
  //
  switch (type) {
    case INT1_KIND:
      return v64;
    case INT8_KIND:
      return (uint8_t) v64;
    case INT16_KIND:
      return (uint16_t) v64;
    case INT24_KIND:
      return (uint32_t) v64;
    case INT32_KIND:
      return (uint32_t) v64;
    case INT64_KIND:
      return (uint64_t) v64;
    case INT80_KIND:
      DEBUG_STDERR("Unsupported type INT80.");
      safe_assert(false);
    default:
      return v64;
  }

}

double IValue::getFlpValue() {
  double v = value.as_flp;

  //
  // return floating-point value depending on type
  //
  switch (type) {
    case FLP32_KIND:
      return (float) v;
    case FLP64_KIND:
      return (double) v;
    case FLP80X86_KIND:
      return v;
    case FLP128_KIND:
    case FLP128PPC_KIND:
      DEBUG_STDERR("Unsupported type FLP128 or FLP128PPC.");
      safe_assert(false);
      return 0;
    default:
      return v;
  }
}

void IValue::create(const IValue& iv) {
  type = iv.getType();
  value = iv.getValue();
  valueOffset = iv.getValueOffset();
  size = iv.getSize();
  index = iv.getIndex();
  firstByte = iv.getFirstByte();
  length = iv.getLength();
  offset = iv.getOffset();
  bitOffset = iv.getOffset();
  lineNumber = iv.getLineNumber();
  scope = iv.getScope();
  shadow = iv.getShadow();
}


// precondition: setting type first
void IValue::setValue(int64_t v) {
  if (isIntValue()) {
    this->value.as_int = v;
  }
  else if (isFlpValue()) {
    double *ptr;
    ptr = (double*)&v;
    this->value.as_flp = *ptr;
  }
  else if (isPtrValue()) {
    this->value.as_ptr = (void*)v;
  }
  else {
    cout << "ERROR" << endl;
    safe_assert(false);
  }
  return;
}
