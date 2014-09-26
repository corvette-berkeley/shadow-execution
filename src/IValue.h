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
#include <memory>

class IValue {

private:
	/**
	 * An IValue is a value manipulated during interpretation. Each IValue
	 * has a corresponding concrete value.
	 *
	 * Fields of an IValue object include:
	 *  value: value of this object, e.g., 10, 5.5, 0x3, etc. For pointers the value is
	 *  the corresponding concrete value.
	 *  shadow: pointer to corresponding shadow object.
	 *  type: type of this object, e.g. int32, float, ptr, etc.
	 *  valueOffset: the difference between the IValue address and the
	 *    corresponding concrete value address (used for pointer value only).
	 *  size: size of the data this pointer points to (used for pointer value only).
	 *  index: the current index of this object (used for array pointer only).
	 *  firstByte: if this object is an element of an array, this is the
	 *    distance from the base pointer to this object (used for elements in an array).
	 *  length: number of elements in the array this pointer points to (used for array pointer only).
	 *  offset: distance from the base pointer to this value (used for pointer value only).
	 *  bitOffset: to represent data not fiting to a byte, values range from 0 to 7.
	 *  scope: either a GLOBAL, LOCAL or REGISTER.
	 */

	VALUE value;
	void* shadow;
	KIND type;
	int64_t valueOffset;
	unsigned size, index, firstByte, length;
	int offset, bitOffset;
	SCOPE scope;
	bool struct_;
	// static long counterNew, counterDelete;

	/**
	 * Define how to copy shadow values. This varies analysis by analysis.
	 *
	 * @param IValue the source IValue to get shadow value from.
	 * @param IValue the destination IValue to copy shadow value to.
	 */
	static void (*copyShadow)(const IValue*, IValue*);

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
	IValue(KIND t, VALUE v, SCOPE s)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0),
		  bitOffset(0), scope(s), struct_(false) {
		// counterNew++;
	}

	IValue(KIND t, VALUE v)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0),
		  bitOffset(0), scope(REGISTER), struct_(false) {
		// counterNew++;
	}

	IValue(KIND t, VALUE v, unsigned fb)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(fb), length(0), offset(0),
		  bitOffset(0), scope(REGISTER), struct_(false) {
		// counterNew++;
	}

	IValue(KIND t, VALUE v, unsigned s, int o, int i, unsigned l)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(s), index(i), firstByte(0), length(l), offset(o),
		  bitOffset(0), scope(REGISTER), struct_(false) {
		// counterNew++;
	}

	IValue(KIND t)
		: shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0), bitOffset(0),
		  scope(REGISTER), struct_(false) {
		value.as_int = 0;
		// counterNew++;
	}

	IValue()
		: shadow(NULL), type(INV_KIND), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0),
		  bitOffset(0), scope(REGISTER), struct_(false) {
		// counterNew++;
	}

	IValue(const IValue& iv) : struct_(false) {
		create(iv);
		// counterNew++;
	};

	~IValue() {
		uncreate();
		// counterDelete++;
	};

	static struct deleter {
		void operator()(IValue* iv) {
			if (iv->isStruct()) {
				delete[] iv;
				return;
			}
			delete iv;
		}
	} del;

	IValue& operator=(const IValue& iv) {
		if (&iv != this) {

			// free the object in the left-hand side
			uncreate();

			// copy elements from the right-hand side to the left-hand side
			create(iv);
			// counterNew++;
		}

		return *this;
	};

	void setType(KIND type) {
		this->type = type;
	};

	void setValue(VALUE value) {
		this->value = value;
	};

	void setValue(int64_t value);

	void setTypeValue(KIND type, VALUE value) {
		this->type = type;
		this->value = value;
	};

	void setTypeValue(KIND type, int64_t value) {
		this->type = type;
		setValue(value);
	};

	void setTypeValueSize(KIND type, VALUE value, unsigned size) {
		this->type = type;
		this->value = value;
		this->size = size;
	};

	void setValueOffset(int64_t valueOffset) {
		this->valueOffset = valueOffset;
	};

	void setScope(SCOPE scope) {
		this->scope = scope;
	};

	void setSize(unsigned int size) {
		this->size = size;
	};

	void setIndex(unsigned index) {
		this->index = index;
	};

	void setFirstByte(unsigned firstByte) {
		this->firstByte = firstByte;
	};

	void setLength(unsigned length) {
		this->length = length;
	};

	void setOffset(int offset) {
		this->offset = offset;
	};

	void setBitOffset(int bitOffset) {
		this->bitOffset = bitOffset;
	};

	void setShadow(void* shadow) {
		this->shadow = shadow;
	};

	static void setCopyShadow(void (*copyShadow)(const IValue*, IValue*)) {
		IValue::copyShadow = copyShadow;
	};

	void setInitialized() {
		if (type == PTR_KIND && length == 0) {
			length = 1;
		}
	};

	void setStruct(bool flag) {
		struct_ = flag;
	}

	void setAll(KIND type, VALUE value, unsigned size, int index, unsigned length, int64_t valueOffset);

	KIND getType() const {
		return this->type;
	};

	VALUE getValue() const {
		return this->value;
	};

	unsigned getIndex() const {
		return this->index;
	};

	unsigned getFirstByte() const {
		return this->firstByte;
	};

	unsigned getLength() const {
		return this->length;
	};

	unsigned int getSize() const {
		return this->size;
	};

	SCOPE getScope() const {
		return this->scope;
	};

	int getOffset() const {
		return this->offset;
	};

	int getBitOffset() const {
		return this->bitOffset;
	};

	int64_t getValueOffset() const {
		return this->valueOffset;
	};

	void* getShadow() const {
		return this->shadow;
	};

	int64_t getIntValue();

	uint64_t getUIntValue();

	void* getPtrValue() {
		return value.as_ptr;
	};

	bool isStruct() {
		return struct_;
	}

	double getFlpValue();

	IValue* getIPtrValue() {
		return (IValue*)((int64_t)value.as_ptr + valueOffset);
	};

	bool isInitialized() const {
		return type != PTR_KIND || length > 0;
	};

	bool isIntValue() {
		return type == INT1_KIND || type == INT8_KIND || type == INT16_KIND || type == INT24_KIND || type == INT32_KIND ||
			   type == INT64_KIND || type == INT80_KIND;
	};

	bool isFlpValue() {
		return type == FLP32_KIND || type == FLP64_KIND || type == FLP80X86_KIND || type == FLP128_KIND ||
			   type == FLP128PPC_KIND;
	};

	bool isPtrValue() {
		return type == PTR_KIND;
	};

	std::string toString() const;

	/**
	 * Copy the content of this IValue to dest IValue.
	 *
	 * @note we do not copy the field firstByte
	 *
	 * @param dest the destination of the copy.
	 */
	void copy(IValue* dest) const;
	void copy(std::unique_ptr<IValue, IValue::deleter>& dest) const {
		copy(dest.get());
	}

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
	bool writeValue(int offset, int byte, const IValue* src);
	bool writeValue(int offset, int byte, const std::unique_ptr<IValue, IValue::deleter>& src) {
		return writeValue(offset, byte, src.get());
	}

	/**
	 * Check whether this is an IValue by comparing its type with its expected
	 * type.
	 *
	 * @param t the expected type of this iValue.
	 *
	 * @return true if this is an IValue; false otherwise.
	 */
	bool isIValue(KIND t);

	static void printCounters() {}

	/*
	static void printCounters() {
	  int diff = counterNew - counterDelete;
	  if (diff) {
	    cout << "LOST: " << diff << " out of " << counterNew << endl;
	  }
	}
	*/

	void clear();
};

#endif /* IVALUE_H_ */
