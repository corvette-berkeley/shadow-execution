#ifndef _BLAME_NODE_H_
#define _BLAME_NODE_H_

#include <vector>

struct BlameNodeID {
	IID iid;
	PRECISION precision;

public:
	BlameNodeID() : iid(0), precision(BITS_FLOAT) {}
	;
	BlameNodeID(IID i, PRECISION p) : iid(i), precision(p) {}
	;

	bool operator<(const BlameNodeID& rhs) const {
		if (iid == rhs.iid) {
			return precision < rhs.precision;
		}

		return iid < rhs.iid;
	}
	;
};

struct BlameNode {
	BlameNodeID id;
	std::vector<BlameNodeID> children;
	bool requireHigherPrecision : 1;
	bool requireHigherPrecisionOperator : 1;

public:
	BlameNode()
		: id(BlameNodeID()), children({
	}),
	requireHigherPrecision(false), requireHigherPrecisionOperator(false) {}
	;
	BlameNode(IID i, PRECISION p, bool rhp, bool rhpo,
			  const std::vector<BlameNodeID>& c)
		: id(BlameNodeID(i, p)), children(c), requireHigherPrecision(rhp),
		  requireHigherPrecisionOperator(rhpo) {}
	;

	bool operator<(const BlameNode& rhs) const {
		return id < rhs.id;
	}
	;
};

#endif
