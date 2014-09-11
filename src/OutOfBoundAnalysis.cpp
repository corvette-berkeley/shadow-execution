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
#include <cstddef>
#include "IValue.h"

void OutOfBoundAnalysis::getelementptr(IID, bool, KVALUE* base, KVALUE* offset, KIND, uint64_t size, bool, int,
									   int line, int inx) {

	IValue* basePtrLocation = executionStack.top()[base->inx];
	IValue* ptrLocation = nullptr;

	bool propagate = isOutOfBound(basePtrLocation);

	if (basePtrLocation->isInitialized()) {
		int offsetValue = offset->inx != -1 ? executionStack.top()[offset->inx]->getValue().as_int : offset->value.as_int;

		unsigned newOffset = (offsetValue * (size / 8)) + basePtrLocation->getOffset();

		unsigned index = findIndex((IValue*)basePtrLocation->getValue().as_ptr, newOffset, basePtrLocation->getLength());
		ptrLocation =
			new IValue(PTR_KIND, basePtrLocation->getValue(), size / 8, newOffset, index, basePtrLocation->getLength());

		if (propagate && isOutOfBound(ptrLocation)) {
			int baseLine = *((int*)basePtrLocation->getShadow());
			cout << line << " : [BUG WARNING] " << endl;
			cout << "\t Potentially invalid memory pointer." << endl;
			cout << "\t Propagated from " << baseLine << "." << endl;
			cout << endl;
		} else if (propagate && !isOutOfBound(ptrLocation)) {
			int baseLine = *((int*)basePtrLocation->getShadow());
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
		ptrLocation = new IValue(PTR_KIND, basePtrLocation->getValue(), size / 8, 0, 0, 0);
	}

	int* shadow = new int(line);
	ptrLocation->setShadow((void*)shadow);
	ptrLocation->setLineNumber(line);

	executionStack.top()[inx] = ptrLocation;

	return;
}

void OutOfBoundAnalysis::getelementptr_array(IID, bool, KVALUE* op, KIND, int, int inx) {

	IValue* ptrArray = executionStack.top()[op->inx];
	IValue* array = static_cast<IValue*>(ptrArray->getValue().as_ptr);

	getElementPtrIndexList.pop();

	unsigned index = 1;
	arraySize.pop();
	while (!arraySize.empty()) {
		index = index * arraySize.front();
		arraySize.pop();
	}
	index = getElementPtrIndexList.front() * index;
	getElementPtrIndexList.pop();
	safe_assert(getElementPtrIndexList.empty());

	index = ptrArray->getIndex() + index;

	IValue arrayElem = array[index];
	IValue* arrayElemPtr = new IValue(PTR_KIND, ptrArray->getValue());
	arrayElemPtr->setIndex(index);
	arrayElemPtr->setLength(ptrArray->getLength());

	if (index < ptrArray->getLength()) {
		arrayElemPtr->setSize(KIND_GetSize(arrayElem.getType()));
		arrayElemPtr->setOffset(arrayElem.getFirstByte());
	}

	if (isOutOfBound(arrayElemPtr)) {
		cout << "[BUG WARNING] Potentially invalid memory pointer!\n" << endl;
	}

	executionStack.top()[inx] = arrayElemPtr;
}

void OutOfBoundAnalysis::getelementptr_struct(IID, bool, KVALUE* op, KIND, KIND, int inx) {
	IValue* structPtr = executionStack.top()[op->inx];
	IValue* structBase = static_cast<IValue*>(structPtr->getValue().as_ptr);


	getElementPtrIndexList.pop();
	unsigned index = getElementPtrIndexList.front();
	getElementPtrIndexList.pop();
	safe_assert(getElementPtrIndexList.empty());
	index = structPtr->getIndex() + index;

	IValue structElem = structBase[index];
	IValue* structElemPtr = new IValue(PTR_KIND, structPtr->getValue());
	structElemPtr->setIndex(index);
	structElemPtr->setLength(structPtr->getLength());

	if (index < structPtr->getLength()) {
		structElemPtr->setSize(KIND_GetSize(structElem.getType()));
		structElemPtr->setOffset(structElem.getFirstByte());
	}

	if (isOutOfBound(structElemPtr)) {
		cout << "[BUG WARNING] Potentially invalid memory pointer!\n" << endl;
	}

	executionStack.top()[inx] = structElemPtr;
}

void OutOfBoundAnalysis::load(IID, KIND type, KVALUE* src, bool, int, int, int line, int inx) {
	IValue* srcPtrLocation = src->isGlobal ? globalSymbolTable[src->inx] : executionStack.top()[src->inx];

	IValue* destLocation = new IValue();

	if (isOutOfBound(srcPtrLocation)) {
		int baseLine = *((int*)srcPtrLocation->getShadow());
		cout << line << " : [BUG WARNING]" << endl;
		cout << "\tPotentially invalid memory load." << endl;
		cout << "\tPropagated from " << baseLine << "." << endl;
	}

	if (srcPtrLocation->isInitialized() && !isOutOfBound(srcPtrLocation)) {
		unsigned srcOffset = srcPtrLocation->getOffset();

		// retrieving source
		IValue* values = (IValue*)srcPtrLocation->getValue().as_ptr;
		unsigned valueIndex = srcPtrLocation->getIndex();
		unsigned currOffset = values[valueIndex].getFirstByte();

		IValue* srcLocation = &values[valueIndex];

		int internalOffset = srcOffset - currOffset;

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
			value.as_ptr = (void*)destLocation;
			srcPtrLocation->setLength(1);
			srcPtrLocation->setValue(value);
			srcPtrLocation->setSize(KIND_GetSize(destLocation->getType()));
		}
	}

	int* shadow = new int(srcPtrLocation->getLineNumber());
	destLocation->setShadow((void*)shadow);
	destLocation->setLineNumber(line);

	executionStack.top()[inx] = destLocation;

	return;
}

void OutOfBoundAnalysis::store(IID, KVALUE* dest, KVALUE* src, int, int line, int) {
	IValue* destPtrLocation = dest->isGlobal ? globalSymbolTable[dest->inx] : executionStack.top()[dest->inx];

	if (isOutOfBound(destPtrLocation)) {
		cout << line << " : [BUG WARNING] Potentially invalid memory store!\n" << endl;
		return;
	}


	unsigned destPtrOffset = destPtrLocation->getOffset();
	IValue* srcLocation = src->inx == -1 ? new IValue(src->kind, src->value) : executionStack.top()[src->inx];

	if (src->inx != -1) {
		destPtrLocation->setShadow(srcLocation->getShadow());
		destPtrLocation->setLineNumber(srcLocation->getLineNumber());
	}

	// retrieve actual destination
	IValue* values = (IValue*)destPtrLocation->getValue().as_ptr;
	unsigned valueIndex = destPtrLocation->getIndex();
	unsigned currOffset = values[valueIndex].getFirstByte();
	int internalOffset = destPtrOffset - currOffset;

	// writing src into dest
	destPtrLocation->writeValue(internalOffset, destPtrLocation->getSize(), srcLocation);

	// just read again to check store
	IValue* writtenValue = new IValue(srcLocation->getType(), destPtrLocation->readValue(internalOffset, src->kind));

	if (!checkStore(writtenValue, src)) {  // destLocation
		cerr << "\tKVALUE: " << KVALUE_ToString(src) << endl;
		cerr << "\tMismatched values found in Store" << endl;
		abort();
	}

	return;
}

// TODO: Change the passed value to be a value or a reference
bool OutOfBoundAnalysis::isOutOfBound(IValue* pointer) {
	if (pointer->getType() != PTR_KIND) {
		return false;
	}
	if (pointer->getIndex() >= pointer->getLength()) {
		return true;
	}

	if (pointer->getIndex() != pointer->getLength() - 1) {
		return false;
	}

	IValue* array = static_cast<IValue*>(pointer->getValue().as_ptr);
	IValue arrayLem = array[pointer->getIndex()];
	unsigned internalOffset = pointer->getOffset() - arrayLem.getFirstByte();
	int address = internalOffset + pointer->getSize();

	return address > KIND_GetSize(arrayLem->getType());
}
