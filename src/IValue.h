/**
 * @file IValue.h
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

#ifndef IVALUE_H_
#define IVALUE_H_

#include "Common.h"

using namespace std;

class IValue {

  private:
    /**
     * An IValue is value interpreted thorought our interpretation. Each IValue
     * has a corresponding concrete value.
     *
     * Fields of an IValue object includes:
     *  type: type of this object, e.g. int32, float, ptr, ...
     *  value: value of this object, e.g. 10, 5.5, 0x3, ...
     *  valueOffset: the differences between the IValue address and the
     *    correspoinding concrete value address [used for pointer value only]
     *  size: size of the data this pointer point to [used for pointer value only] 
     *  offset: distance from the value to the base pointer [used for pointer value only]
     *  bitOffset: to represent data not fiting to a byte, values range from 0 to 7
     *  index: the current index of this object [used for array (pointer) only] 
     *  firstByte: if this object is an element in an array, this is the
     *    distance from its address to the base pointer [used for elements in an array]
     *  length: number of elements in the array object [used for array (pointer) only]
     *  scope: either a GLOBAL, LOCAL or REGISTER object
     *  fileNumber: file id for source location
     *  lineNumber: source code line number
     *  flag: machine flag (currently unused)
     *  shadow: pointer to shadow object
     */
    KIND type; 
    VALUE value; 
    int64_t valueOffset; 
    unsigned size, index, firstByte, length; 
    int offset, bitOffset, fileNumber, lineNumber; 
    SCOPE scope; 
    MACHINEFLAG flag;
    void* shadow;

    /**
     * Define how to copy shadow values. This varies analysis by analysis.
     *
     * @param IValue the source IValue to get shadow value from.
     * @param IValue the destination IValue to copy shadow value to.
     */
    static void (*copyShadow)(IValue*, IValue*);

    /**
     * Write a chunk of byte to value. This functions returns the actual number
     * of byte written.
     *
     * @note the actual number of byte written is min(maxOffset-offset+1, byte)
     * where maxOffset is size of this value in byte minus one.
     *
     * @param offset the offset to start writing to.
     * @param byte the expected number of byte to write to.
     * @param content the content to read from.
     */
    int setValue(int offset, int byte, uint8_t* content);

    void create(const IValue& iv);

    void uncreate() {};

 public:
    IValue(KIND t, VALUE v, SCOPE s): type(t), value(v), valueOffset(-1),
      size(0), index(0), firstByte(0), length(0), offset(0), bitOffset(0),
      fileNumber(0), lineNumber(0), scope(s), shadow(NULL) {}
      
    IValue(KIND t, VALUE v): type(t), value(v), valueOffset(-1), size(0),
      index(0), firstByte(0), length(0), offset(0), bitOffset(0), 
      fileNumber(0), lineNumber(0), scope(REGISTER), shadow(NULL) {}

    IValue(KIND t, VALUE v, unsigned fb): type(t), value(v), valueOffset(-1),
      size(0), index(0), firstByte(fb), length(0), offset(0), bitOffset(0),
      fileNumber(0), lineNumber(0), scope(REGISTER), shadow(NULL) {}

    IValue(KIND t, VALUE v, unsigned s, int o, int i, unsigned l): type(t),
      value(v), valueOffset(-1), size(s), index(i), firstByte(0), length(l),
      offset(o), bitOffset(0), fileNumber(0), lineNumber(0), scope(REGISTER), shadow(NULL) {}
    
    IValue(KIND t): type(t), valueOffset(-1), size(0), index(0), firstByte(0),
      length(0), offset(0), bitOffset(0), fileNumber(0), lineNumber(0), scope(REGISTER),
      shadow(NULL) { value.as_int = 0; }

    IValue(): type(INV_KIND), valueOffset(-1), size(0), index(0), firstByte(0),
      length(0), offset(0), bitOffset(0), fileNumber(0), lineNumber(0), scope(REGISTER),
      shadow(NULL) {}

    IValue(const IValue& iv) {
      create(iv);
    };

    ~IValue() {
      uncreate();
    };

    IValue& operator=(const IValue& iv) {
      if (&iv != this) {
        
        // free the object in the left-hand side
        uncreate();

        // copy elements from the right-hand side to the left-hand side
        create(iv);
      }

      return *this;
    };

    void setType(KIND type) { this->type = type; };

    void setValue(VALUE value) { this->value = value; };

    void setValueOffset(int64_t valueOffset) { this->valueOffset = valueOffset; };

    void setScope(SCOPE scope) { this->scope = scope; };

    void setSize(unsigned int size) { this->size = size; };

    void setIndex(unsigned index) { this->index = index; };

    void setFirstByte(unsigned firstByte) { this->firstByte = firstByte; };

    void setLength(unsigned length) { this->length = length; };

    void setOffset(int offset) { this->offset = offset; };

    void setBitOffset(int bitOffset) { this->bitOffset = bitOffset; };

    void setLineNumber(int lineNumber) { this->lineNumber = lineNumber; };

    void setFileNumber(int fileNumber) { this->fileNumber = fileNumber; };

    void setSourceInfo(int fileNumber, int lineNumber) { this->fileNumber = fileNumber; this->lineNumber = lineNumber;};

    void setShadow(void* shadow) { this->shadow = shadow; };

    static void setCopyShadow(void (*copyShadow)(IValue*, IValue*)) {
      IValue::copyShadow = copyShadow; };

    void setInitialized() {
      if (type == PTR_KIND && length == 0) {
        length = 1;
      }
    };

    KIND getType() const { return this->type; };

    VALUE getValue() const { return this->value; };

    unsigned getIndex() const { return this->index; };

    unsigned getFirstByte() const { return this->firstByte; };

    unsigned getLength() const { return this->length; };

    unsigned int getSize() const { return this->size; };

    SCOPE getScope() const { return this->scope; };

    int getOffset() const { return this->offset; };

    int getBitOffset() const { return this->bitOffset; };

    int getLineNumber() const { return this->lineNumber; };

    int64_t getValueOffset() const { return this->valueOffset; };

    void* getShadow() const { return this->shadow; };

    int64_t getIntValue();

    uint64_t getUIntValue();

    void* getPtrValue() { return value.as_ptr; };

    double getFlpValue();

    void* getIPtrValue() { return (void*)((int64_t)value.as_ptr + valueOffset); };

    bool isInitialized() { return type != PTR_KIND || length > 0; };

    bool isIntValue() { return type == INT1_KIND || type == INT8_KIND || type
      == INT16_KIND || type == INT24_KIND || type == INT32_KIND || type ==
        INT64_KIND || type == INT80_KIND; };

    bool isFlpValue() { return type == FLP32_KIND || type == FLP64_KIND || type
      == FLP80X86_KIND || type == FLP128_KIND || type == FLP128PPC_KIND; };

    bool isPtrValue() { return type == PTR_KIND; };

    string toString();

    /**
     * Copy the content of this IValue to dest IValue.
     *
     * @note we do not copy the field firstByte
     *
     * @param dest the destination of the copy.
     */
    void copy(IValue* dest);

    /**
     * Copy the content of kValue to this IValue.
     *
     * @param kValue the kvalue to copy from.
     */
    void copyFrom(KVALUE* kValue);

    /**
     * Read a chunk of byte from a pointer value, given the offset to read from
     * and the number of bytes to read.
     *
     * @note This function is specialized for pointer value. 
     *
     * @param offset the offset to start reading from.
     * @param byte the number of bytes to read.
     *
     * @return a VALUE object that wrap all the bytes readed.
     */
    VALUE readValue(int offset, KIND type);

    /**
     * Write a chunk of byte to a pointer value, given the offset to start
     * writing to, the number of bytes to write, and the content to write.
     *
     * This function updates the current pointer value.
     *
     * @note This function is specialized for pointer value.
     *
     * @param offset the offset to start writing to.
     * @param byte the number of bytes to write.
     * @param src the source to get value form.
     *
     * @return true if this is a trivial write; false otherwise
     */
    bool writeValue(int offset, int byte, IValue* src);

    /**
     * Check whether this is an IValue by comparing its type with its expected
     * type.
     *
     * @param t the expected type of this iValue.
     *
     * @return true if this is an IValue; false otherwise.
     */
    bool isIValue(KIND t);
};

#endif /* IVALUE_H_ */
