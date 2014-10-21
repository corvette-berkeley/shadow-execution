#ifndef _BLAME_SHADOW_OBJECT_H_
#define _BLAME_SHADOW_OBJECT_H_

#include <array>
#include "BlameUtilities.h"

struct BlameShadowObject {
	IID id; // LLVM instruction id.
	std::array<HIGHPRECISION, PRECISION_NO - 1> highValues;
	LOWPRECISION lowValue;

	BlameShadowObject() : id(0), lowValue(0) {}

	BlameShadowObject(IID i, std::array<HIGHPRECISION, PRECISION_NO - 1> hs,
					  LOWPRECISION l)
		: id(i), highValues(hs), lowValue(l) {}
	;

	BlameShadowObject& operator=(const BlameShadowObject& rhs) {
		if (&rhs != this) {
			id = rhs.id;
			highValues = rhs.highValues;
			lowValue = rhs.lowValue;
		}
		return *this;
	}

};

#endif
