#ifndef _BLAME_SHADOW_OBJECT_H_
#define _BLAME_SHADOW_OBJECT_H_

#include <array>
#include "BlameUtilities.h"

struct BlameShadowObject {
	IID id;
	HIGHPRECISION highValue;

	BlameShadowObject() : id(0), highValue(0) {}

	BlameShadowObject(IID i, HIGHPRECISION h) : id(i), highValue(h) {};

	BlameShadowObject& operator=(const BlameShadowObject& rhs) {
		if (&rhs != this) {
			id = rhs.id;
			highValue = rhs.highValue;
		}
		return *this;
	}
};

#endif
