#ifndef _BLAME_SHADOW_OBJECT_H_
#define _BLAME_SHADOW_OBJECT_H_

#include <array>
#include "BlameUtilities.h"

struct BlameShadowObject {
	IID id; // LLVM instruction id.
	std::array<HIGHPRECISION, PRECISION_NO> values;

	BlameShadowObject() : id(0) {}

	BlameShadowObject(IID i, std::array<HIGHPRECISION, PRECISION_NO> v)
		: id(i), values(v) {}
	;

	BlameShadowObject& operator=(const BlameShadowObject& rhs) {
		if (&rhs != this) {
			id = rhs.id;
			values = rhs.values;
		}
		return *this;
	}

};

#endif
