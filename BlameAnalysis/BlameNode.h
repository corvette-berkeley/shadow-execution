/**
 * @file BlameNode.h
 * @brief BlameNode Declarations.
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
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen

#ifndef BLAME_NODE_H
#define BLAME_NODE_H

#include "BlameNodeID.h"
#include "BlameTreeUtilities.h"

/**
 * BlameNode is abstraction of a dynamic instruction in the program.
 *
 * We make an assumption that the program is in three-address-code, therefore,
 * each BlameNode is also associated with a value of the program. Each
 * BlameNode has a unique dynamic program counter, a (not neccessarily
 * unique) source code program counter, and a precision constraint associated
 * with the
 * value it carries. Each blame node can blame (connect to) a set of node group
 * that decides its precision constraint.
 */
struct BlameNode {
	int dpc; // dynamic program counter of instruction associated with this blame
	// tree node
	int pc; // source program counter of instruction associated with this blame
	// tree noe
	int fid; // id of source file containing instruction associated with this
	// blame tree node
	bool highlight; // highlighted node indicates higher precision requirement
	PRECISION precision; // the precision constraint of this blame tree node
	vector<vector<BlameNodeID>> edges; // set of nodes that this
	// node blames, a node is identified
	// by a pair of dpc and precision
	vector<bool> edgeAttributes;        // set of edge attributes
	// right now it indicates whether
	// the edge (the computation)
	// requires high precision

	BlameNode(int dp = 0, int p = 0, int f = 0, bool hl = false,
	PRECISION prec = BITS_FLOAT, vector<vector<BlameNodeID>> es = {
	},
	vector<bool> eas = {
	})
		: dpc(dp), pc(p), fid(f), highlight(hl), precision(prec), edges(es),
		  edgeAttributes(eas) {}
	;

	/**
	 * Visualize this node in GraphViz dot format.
	 */
	std::string toDot() const;

	/**
	 * Visualize edges associate with this ndoe in GraphViz dot format.
	 */
	std::string edgeToDot(const map<BlameNodeID, BlameNode>& nodes) const;
};

#endif /* BLAME_NODE_H */
