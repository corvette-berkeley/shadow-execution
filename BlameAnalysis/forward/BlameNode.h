// Author: Cuong Nguyen

#ifndef _BLAME_NODE_H_
#define _BLAME_NODE_H_

#include "BlameUtilities.h"

struct BlameNode {
	IID iid;  // LLVM instruction id.
	PRECISION precision;
	bool requireHigherPrecision;
	bool requireHigherPrecisionOperator;
	std::vector<BlameNode> children;

	BlameNode(IID i, PRECISION p, bool rhp, bool rhpo, std::vector<BlameNode> c)
		: iid(i), precision(p), requireHigherPrecision(rhp), requireHigherPrecisionOperator(rhpo), children(c) {};

	BlameNode& operator=(const BlameNode& rhs) {
		if (&rhs != this) {
			iid = rhs.iid;
			precision = rhs.precision;
			requireHigherPrecision = rhs.requireHigherPrecision;
			requireHigherPrecisionOperator = rhs.requireHigherPrecisionOperator;
			children = rhs.children;
		}
		return *this;
	}
};

#endif
