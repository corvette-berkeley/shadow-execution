// Author: Cuong Nguyen

#ifndef _BLAME_NODE_H_
#define _BLAME_NODE_H_

#include "BlameUtilities.h"

struct BlameNode {
	std::array<BlameNode*, 2> children;
	IID iid;  // LLVM instruction id.
	PRECISION precision;
	int size : 3;
	bool requireHigherPrecision : 1;
	bool requireHigherPrecisionOperator : 1;

public:
	BlameNode(IID i, PRECISION p, bool rhp, bool rhpo, const array<BlameNode*, 2>& c)
		: children(c), iid(i), precision(p), size(c[0] ? (c[1] ? 2 : 1) : 0), requireHigherPrecision(rhp),
		  requireHigherPrecisionOperator(rhpo) {}

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

	bool operator<(const BlameNode& rhs) const {
		if (iid == rhs.iid) {
			return precision < rhs.precision;
		}

		return iid < rhs.iid;
	}

	int getSize() const {
		return size;
	}

	auto cbegin() const -> decltype(children.cbegin()) {
		return children.begin();
	}

	auto cend() const -> decltype(children.cbegin() + size) {
		return children.cbegin() + size;
	}

	void setChildren(std::array<BlameNode*, 2> ar) {
		children = ar;
		size = ar[0] ? (ar[1] ? 2 : 1) : 0;
	}
};

#endif
