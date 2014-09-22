// Author: Cuong Nguyen

#ifndef _BLAME_SHADOW_OBJECT_H_
#define _BLAME_SHADOW_OBJECT_H_

#include "BlameUtilities.h"

struct BlameShadowObject {
	HIGHPRECISION highValue;
	LOWPRECISION lowValue;

	BlameShadowObject(HIGHPRECISION h, LOWPRECISION l) : highValue(h), lowValue(l) {};
};

#endif
