// Author: Cuong Nguyen
// :xa
//

#ifndef _BLAME_SHADOW_OBJECT_H_
#define _BLAME_SHADOW_OBJECT_H_

#include "BlameUtilities.h"

struct BlameShadowObject {
	IID id; // LLVM instruction id.
	HIGHPRECISION highValue;
	LOWPRECISION lowValue;

	BlameShadowObject(IID i, HIGHPRECISION h, LOWPRECISION l)
		: id(i), highValue(h), lowValue(l) {}
	;

	BlameShadowObject& operator=(const BlameShadowObject& rhs) {
		if (&rhs != this) {
			id = rhs.id;
			highValue = rhs.highValue;
			lowValue = rhs.lowValue;
		}
		return *this;
	}

};

#endif
