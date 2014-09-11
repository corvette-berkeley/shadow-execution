// Author: Cuong Nguyen

#include "BlameNode.h"
#include <sstream>

std::string BlameNode::toDot() {
	std::ostringstream dot;

	dot << "\"(" << dpc << ", " << pc << ", "
		<< BlameTreeUtilities::precisionToString(precision) << ")\"";

	return dot.str();
}

std::string BlameNode::edgeToDot(map<BlameNodeID, BlameNode> nodes) {
	int tempNodeCnt = 0;
	std::ostringstream dot, tmpDot;

	tmpDot << "\"tmp" << dpc << "-"
		   << BlameTreeUtilities::precisionToString(precision);

	safe_assert(edges.size() == edgeAttributes.size());
	for (std::pair<vector<vector<BlameNodeID> >::iterator, vector<bool>::iterator>
			it(edges.begin(), edgeAttributes.begin());
			it.first != edges.end(); ++it.first, ++it.second) {

		vector<BlameNodeID> bnIDs = *it.first;
		bool edgeAttr = *it.second;
		std::ostringstream tmpNodeStr;

		tmpNodeStr << tmpDot.str() << "-" << tempNodeCnt << "\"";
		if (edgeAttr) {
			// Edge that performs in high precision.
			dot << tmpNodeStr.str() << "[style=dotted, shape=diamond, color=red]"
				<< endl;
		} else {
			// Edge that performs in low precision.
			dot << tmpNodeStr.str() << "[style=dotted, shape=diamond]" << endl;
		}
		dot << "\t" << toDot() << " -> " << tmpNodeStr.str() << endl;

		for (vector<BlameNodeID>::iterator nodesIt = bnIDs.begin();
				nodesIt != bnIDs.end(); ++nodesIt) {
			BlameNodeID blameNodeID = *nodesIt;
			BlameNode blameNode = nodes[blameNodeID];

			dot << "\t" << tmpNodeStr.str() << " -> " << blameNode.toDot() << endl;
		}

		tempNodeCnt++;
	}

	return dot.str();
}
