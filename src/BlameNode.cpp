/**
 * @file BlameNode.cpp
 * @brief
 */

/*
 * Copyright (c) 2013, UC Berkeley All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1.  Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the UC Berkeley nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY UC BERKELEY ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL UC BERKELEY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen

#include "BlameNode.h"
#include <sstream>

using std::string;
using std::ostringstream;
using std::map;
using std::make_pair;

string BlameNode::toDot() const {
	ostringstream dot;

	dot << "\"(" << dpc << ", " << pc << ", " << BlameTreeUtilities::precisionToString(precision) << ")\"";

	return dot.str();
}

// TODO: why are BlacmeNodeID's in existance?
string BlameNode::edgeToDot(const map<BlameNodeID, BlameNode>& nodes) const {
	ostringstream tmpDot;
	ostringstream dot;

	int tempNodeCnt = 0;
	tmpDot << "\"tmp" << dpc << "-" << BlameTreeUtilities::precisionToString(precision);

	safe_assert(edges.size() == edgeAttributes.size());
	for (auto it = make_pair(edges.begin(), edgeAttributes.begin()); it.first != edges.end(); ++it.first, ++it.second) {
		vector<BlameNodeID> bnIDs = *it.first;
		bool edgeAttr = *it.second;

		ostringstream tmpNodeStr;
		tmpNodeStr << tmpDot.str() << "-" << tempNodeCnt << "\"";
		if (edgeAttr) {
			// edge perform in high precision
			dot << tmpNodeStr.str() << "[style=dotted, shape=diamond, color=red]" << endl;
		} else {
			// edge perform in high precision
			dot << tmpNodeStr.str() << "[style=dotted, shape=diamond]" << endl;
		}
		dot << "\t" << toDot() << " -> " << tmpNodeStr.str() << endl;

		for (auto blameNodeID : bnIDs) {
			auto it = nodes.find(blameNodeID);
			if (it != nodes.end()) {
				const BlameNode& blameNode = it->second;
				dot << "\t" << tmpNodeStr.str() << " -> " << blameNode.toDot() << endl;
			} else {
				const BlameNode blameNode;
				dot << "\t" << tmpNodeStr.str() << " -> " << blameNode.toDot() << endl;
			}
		}

		tempNodeCnt++;
	}

	return dot.str();
}
