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
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen and Cindy Rubio-Gonzalez

#ifndef IVALUE_H_
#define IVALUE_H_

#include <functional>
#include "Common.h"

class IValue {

private:
	/**
	 * An IValue is a value manipulated during interpretation. Each IValue
	 * has a corresponding concrete value.
	 *
	 * Fields of an IValue object include:
	 *  value: value of this object, e.g., 10, 5.5, 0x3, etc. For pointers the
	value is
	 *  the corresponding concrete value.
	 *  shadow: pointer to corresponding shadow object.
	 *  type: type of this object, e.g. int32, float, ptr, etc.
	 *  valueOffset: the difference between the IValue address and the
	 *    corresponding concrete value address (used for pointer value only).
	 *  size: size of the data this pointer points to (used for pointer value
	only).
	 *  index: the current index of this object (used for array pointer only).
	 *  firstByte: if this object is an element of an array, this is the
	 *    distance from the base pointer to this object (used for elements in an
	array).
	 *  length: number of elements in the array this pointer points to (used for
	array pointer only).
	 *  offset: distance from the base pointer to this value (used for pointer
	value only).
	 *  bitOffset: to represent data not fiting to a byte, values range from 0 to
	7.
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

	// set to be true only when we are certain that we own
	// the data returned by getIPtrValue()
	// must be set to false whenever value is updated
	// unless we can PROVE that it is done safely.
	// (note that proof should be expressed in C++
	// not half-written somewhere else)
	bool owns_ptr;

	// static long counterNew, counterDelete;

	/**
	 * Define how to copy shadow values. This varies analysis by analysis.
	 *
	 * param IValue the source IValue to get shadow value from.
	 * param IValue the destination IValue to copy shadow value to.
	 */
	static std::function<void* (void*)> copyShadow;
	static std::function<void(void*)> deleteShadow;

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

public:
	explicit IValue(KIND t, int number_of_type, void* concrete_address, KIND k = PTR_KIND, SCOPE s = REGISTER)
		: value(concrete_address), shadow(nullptr), type(k),
		  valueOffset(reinterpret_cast<intptr_t>(new IValue[number_of_type]) -
					  reinterpret_cast<intptr_t>(concrete_address)),
		  size(KIND_GetSize(t)), index(0), firstByte(0), length(number_of_type), offset(0), bitOffset(0), scope(s),
		  struct_(false), owns_ptr(true) {
		IValue* location = reinterpret_cast<IValue*>(reinterpret_cast<intptr_t>(value.as_ptr) + valueOffset);
		unsigned firstByte = 0, bitOffset = 0;
		for (int i = 0; i < number_of_type; ++i) {
			location[i] = IValue(0, i, (firstByte + bitOffset) / 8, bitOffset % 8, t);
			firstByte += KIND_GetSize(t);
			// WHY ARE WE DOUBLE INCREMENTING HERE?
			bitOffset = (t == INT1_KIND) ? bitOffset + 1 : bitOffset;
			/*if (t == INT1_KIND) {
			  bitOffset++;
			}*/
		}
	}

	explicit IValue(const std::vector<KIND>& collection, void* concrete_address, KIND k = PTR_KIND, SCOPE s = REGISTER)
		: value(concrete_address), shadow(nullptr), type(k),
		  valueOffset(reinterpret_cast<intptr_t>(new IValue[collection.size()]) -
					  reinterpret_cast<intptr_t>(concrete_address)),
		  size(KIND_GetSize(collection.size() > 0 ? collection.front() : PTR_KIND)), index(0), firstByte(0),
		  length(collection.size()), offset(0), bitOffset(0), scope(s), struct_(false), owns_ptr(true) {
		unsigned firstByte = 0, bitOffset = 0;
		IValue* locArr = reinterpret_cast<IValue*>(reinterpret_cast<intptr_t>(value.as_ptr) + valueOffset);
		// TODO: add in assert that collection is only made up on primitive types (not an array? or struct)
		for (uint64_t i = 0; i < collection.size(); i++) {
			locArr[i] = IValue(0, i, (firstByte + bitOffset) / 8, bitOffset % 8, collection[i]);
			firstByte += KIND_GetSize(collection[i]);
			// WHY ARE WE DOUBLE INCREMENTING HERE?
			bitOffset = (collection[i] == INT1_KIND) ? bitOffset + 1 : bitOffset;
			/*if (collection[i] == INT1_KIND) {
			  bitOffset++;
			}*/
		}
	}

	explicit IValue(INT value_, unsigned index_, unsigned firstByte_, unsigned bitOffset_, KIND type_, SCOPE s = REGISTER)
		: value(value_), shadow(nullptr), type(type_), valueOffset(-1), size(0), index(index_), firstByte(firstByte_),
		  length(0), offset(0), bitOffset(bitOffset_), scope(s), owns_ptr(false) {}

	IValue(KIND t, VALUE v, SCOPE s)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0),
		  bitOffset(0), scope(s), struct_(false), owns_ptr(false) {
		// counterNew++;
	}

	IValue(KIND t, VALUE v)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0),
		  bitOffset(0), scope(REGISTER), struct_(false), owns_ptr(false) {
		// counterNew++;
	}

	IValue(KIND t, VALUE v, unsigned fb)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(fb), length(0), offset(0),
		  bitOffset(0), scope(REGISTER), struct_(false), owns_ptr(false) {
		// counterNew++;
	}

	IValue(KIND t, VALUE v, unsigned s, int o, unsigned i, unsigned l)
		: value(v), shadow(NULL), type(t), valueOffset(-1), size(s), index(i), firstByte(0), length(l), offset(o),
		  bitOffset(0), scope(REGISTER), struct_(false), owns_ptr(false) {
		// counterNew++;
	}

	IValue(KIND t)
		: shadow(NULL), type(t), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0), bitOffset(0),
		  scope(REGISTER), struct_(false), owns_ptr(false) {
		value.as_int = 0;
		// counterNew++;
	}

	IValue()
		: shadow(NULL), type(INV_KIND), valueOffset(-1), size(0), index(0), firstByte(0), length(0), offset(0),
		  bitOffset(0), scope(REGISTER), struct_(false), owns_ptr(false) {
		// counterNew++;
	}

	IValue(const IValue& iv)
		: value(iv.getValue()), shadow(copyShadow(iv.getShadow())), type(iv.getType()), valueOffset(iv.getValueOffset()),
		  size(iv.getSize()), index(iv.getIndex()), firstByte(iv.getFirstByte()), length(iv.getLength()),
		  offset(iv.getOffset()), bitOffset(iv.getOffset()), scope(iv.getScope()), struct_(iv.isStruct()),
		  owns_ptr(false) {}  // owns_ptr is false here since they both refer to the same value

public:
	IValue(IValue&& iv) {
		swap(iv);
	}

	~IValue() {
		deleteShadow(shadow);
		clearValue();
	}

private:
	void clearValue() {
		if (owns_ptr) {
			delete[](IValue*)((int64_t)value.as_ptr + valueOffset);
		}
		owns_ptr = false;
		// TODO: implement destruction of pointed to object
	}

public:
	IValue& operator=(const IValue& iv) {
		IValue temp(iv);
		swap(temp);
		return *this;
	}


	IValue& operator=(IValue&& iv) {
		swap(iv);
		return *this;
	}

	void swap(IValue& iv) {
		std::swap(type, iv.type);
		std::swap(value, iv.value);
		std::swap(valueOffset, iv.valueOffset);
		std::swap(size, iv.size);
		std::swap(index, iv.index);
		std::swap(firstByte, iv.firstByte);
		std::swap(length, iv.length);
		std::swap(offset, iv.offset);
		std::swap(bitOffset, iv.bitOffset);
		std::swap(scope, iv.scope);
		std::swap(struct_, iv.struct_);
		std::swap(shadow, iv.shadow);
		std::swap(owns_ptr, iv.owns_ptr);
	}

	void setType(KIND t) {
		type = t;
	}

	void setValue(int64_t value);

	void setValue(VALUE v) {
		clearValue();
		value = v;
	}

	void setTypeValue(KIND t, VALUE v) {
		type = t;
		setValue(v);
	}

	void setTypeValue(KIND t, int64_t v) {
		type = t;
		setValue(v);
	}

	void setTypeValueSize(KIND t, VALUE v, unsigned s) {
		type = t;
		setValue(v);
		size = s;
	}

	void setValueOffset(int64_t vo) {
		owns_ptr = false;
		valueOffset = vo;
	}

	void setScope(SCOPE sc) {
		scope = sc;
	}

	void setSize(unsigned int s) {
		size = s;
	}

	void setIndex(unsigned i) {
		index = i;
	}

	void setFirstByte(unsigned fb) {
		firstByte = fb;
	}

	void setLength(unsigned l) {
		owns_ptr = false;
		length = l;
	}

	void setOffset(int o) {
		offset = o;
	}

	void setBitOffset(int bo) {
		bitOffset = bo;
	}

	void resetShadow() {
		if (shadow) {
			IValue::deleteShadow(shadow);
		}
		shadow = nullptr;
	}

	void takeShadow(IValue& other) {
		shadow = other.shadow;
		other.shadow = nullptr;
	}

	template <typename T> void setShadow(T&& sh) {
		static_assert(!std::is_pointer<T>::value, "We do not allow pointer to pointer types!");
		if (!shadow) {
			shadow = new T(std::move(sh));
		} else {
			std::swap(*getShadow<T>(), sh);
		}
	}

	static void setShadowHandlers(std::function<void* (void*)> cSh, std::function<void(void*)> dSh) {
		IValue::copyShadow = cSh;
		IValue::deleteShadow = dSh;
	}

	void setInitialized() {
		if (type == PTR_KIND && length == 0) {
			length = 1;
		}
	}

	void setStruct(bool flag) {
		struct_ = flag;
	}

	void setAll(KIND type, VALUE value, unsigned size, int index, unsigned length, int64_t valueOffset);

	KIND getType() const {
		return this->type;
	}

	VALUE getValue() const {
		return this->value;
	}

	unsigned getIndex() const {
		return this->index;
	}

	unsigned getFirstByte() const {
		return this->firstByte;
	}

	unsigned getLength() const {
		return this->length;
	}

	unsigned int getSize() const {
		return this->size;
	}

	SCOPE getScope() const {
		return this->scope;
	}

	int getOffset() const {
		return this->offset;
	}

	int getBitOffset() const {
		return this->bitOffset;
	}

	int64_t getValueOffset() const {
		return this->valueOffset;
	}

	void* getShadow() const {
		return this->shadow;
	}

	template <typename T> T* getShadow() const {
		return static_cast<T*>(shadow);
	}

	int64_t getIntValue();

	uint64_t getUIntValue();

	void* getPtrValue() const {
		return value.as_ptr;
	}

	bool isStruct() const {
		return struct_;
	}

	double getFlpValue();

	IValue& getIPtrValue(unsigned index = 0) const {
		IValue* iptr = (IValue*)((int64_t)value.as_ptr + valueOffset);
		return iptr[index];
	}

	const IValue* cbegin() const {
		return (IValue*)((int64_t)value.as_ptr + valueOffset);
	}

	const IValue* cend() const {
		return (IValue*)((int64_t)value.as_ptr + valueOffset) + getLength();
	}

	bool isInitialized() const {
		return type != PTR_KIND || length > 0;
	}

	bool isIntValue() const {
		return type == INT1_KIND || type == INT8_KIND || type == INT16_KIND || type == INT24_KIND || type == INT32_KIND ||
			   type == INT64_KIND || type == INT80_KIND;
	}

	bool isFlpValue() const {
		return type == FLP32_KIND || type == FLP64_KIND || type == FLP80X86_KIND || type == FLP128_KIND ||
			   type == FLP128PPC_KIND;
	}

	bool isPtrValue() const {
		return type == PTR_KIND;
	}

	std::string toString() const;

	/**
	 * Copy the content of this IValue to dest IValue.
	 *
	 * @note we do not copy the field firstByte
	 *
	 * @param dest the destination of the copy.
	 */
	void copy(IValue* dest) const;

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
