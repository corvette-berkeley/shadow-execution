#ifndef _VALUE_H_
#define _VALUE_H_

#include <array>
#include "BlameUtilities.h"

struct Value {
	IID id;
	HIGHPRECISION highValue;

	Value() : id(0), highValue(0) {}

	Value(IID i, HIGHPRECISION h) : id(i), highValue(h) {};

	Value& operator=(const Value& rhs) {
		if (&rhs != this) {
			id = rhs.id;
			highValue = rhs.highValue;
		}
		return *this;
	}
};

#endif
