#ifndef _BLAME_SHADOW_OBJECT_H_
#define _BLAME_SHADOW_OBJECT_H_

#include <array>
#include "BlameUtilities.h"

struct BlameShadowObject {
	IID id; // LLVM instruction id.
	LOWPRECISION lowValue;
	HIGHPRECISION highValue;

	BlameShadowObject() : id(0), lowValue(0), highValue(0) {}

	BlameShadowObject(IID i, LOWPRECISION l, HIGHPRECISION h)
		: id(i), lowValue(l), highValue(h) {}
	;

	BlameShadowObject& operator=(const BlameShadowObject& rhs) {
		if (&rhs != this) {
			id = rhs.id;
			lowValue = rhs.lowValue;
			highValue = rhs.highValue;
		}
		return *this;
	}

};

#endif
