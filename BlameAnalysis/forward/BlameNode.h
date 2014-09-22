// Author: Cuong Nguyen

#ifndef _BLAME_NODE_H_
#define _BLAME_NODE_H_

#include "BlameUtilities.h"
#include "../../src/Common.h"

struct BlameNode {
	IID iid;  // LLVM instruction id.
	PRECISION precision;
	std::vector<BlameNode> children;

	BlameNode(IID i, PRECISION p, std::vector<BlameNode> c) : iid(i), precision(p), children(c) {};
};

#endif
