/**
 * @file BlameTreeAnalysis.cpp
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

#include "BlameTreeAnalysis.h"

BlameNode BlameTreeAnalysis::constructFuncBlameNode(BlameTreeShadowObject<HIGHPRECISION> left, PRECISION precision,
		BlameTreeShadowObject<HIGHPRECISION> right) {
	int dpc;
	std::map<BlameNodeID, BlameNode>::iterator it;

	dpc = left.getDPC();
	BlameNodeID bnID(dpc, precision);
	it = nodes.find(bnID);

	//
	// nodes remember all nodes that are processed before
	// process again only if this is not processed before
	//
	if (it == nodes.end()) {
		//
		// variables declaration
		//
		int pc, fid, dpc01;
		HIGHPRECISION value;
		string func;
		PRECISION i;
		vector<vector<BlameNodeID>> blameNodeIds;
		vector<bool> edgeAttributes;

		//
		// variables definition
		//
		pc = left.getPC();
		fid = left.getFileID();
		value = left.getValue(precision);
		func = left.getFunc();

		//
		// construct a node associate with the function result
		//
		BlameNode node(dpc, pc, fid, false, precision, blameNodeIds, edgeAttributes);

		dpc01 = right.getDPC();

		//
		// try different combination of precision of argument
		//
		for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
			HIGHPRECISION value01 = right.getValue(i);
			HIGHPRECISION lowvalue01 = right.getValue(BITS_FLOAT);

			safe_assert(value == BlameTreeUtilities::clearBits(value, 52 - BlameTreeUtilities::exactBits(precision)));
			if (BlameTreeUtilities::equalWithPrecision(value, BlameTreeUtilities::evalFunc(value01, func), precision)) {
				/*
				if (BlameTreeUtilities::clearBits(value, 52 -
				    BlameTreeUtilities::exactBits(precision)) ==
				  BlameTreeUtilities::clearBits(BlameTreeUtilities::evalFunc(value01,
				      func), 52 - BlameTreeUtilities::exactBits(precision))) {
				      */
				//
				// Construct edges for each blame
				//
				BlameNodeID bnID01(dpc01, i);
				vector<BlameNodeID> blamedNodes;

				//
				// Blame only if the operand cannot be in the lowest precision (right
				// now BITS_23)
				//
				if (i != BITS_FLOAT &&
						BlameTreeUtilities::clearBits(lowvalue01, 52 - BlameTreeUtilities::exactBits(i)) != value01) {
					blamedNodes.push_back(bnID01);
				}

				node.edges.push_back(blamedNodes);

				node.edgeAttributes.push_back(false);

				//
				// Do not try larger i because it subsumes what have been tried
				//
				break;
			}
		}

		//
		// Determined whether node is highlighted
		//
		if (value != (LOWPRECISION)value) {
			node.highlight = true;
		}

		nodes[bnID] = node;

		return node;
	}

	return it->second;
}

BlameNode BlameTreeAnalysis::constructBlameNode(BlameTreeShadowObject<HIGHPRECISION> left, PRECISION precision,
		BlameTreeShadowObject<HIGHPRECISION> right01,
		BlameTreeShadowObject<HIGHPRECISION> right02) {

	int dpc;
	std::map<BlameNodeID, BlameNode>::iterator it;

	dpc = left.getDPC();
	BlameNodeID bnID(dpc, precision);
	it = nodes.find(bnID);

	//
	// nodes remember all nodes that are processed before
	// process again only if this is not processed before
	//
	if (it == nodes.end()) {
		//
		// variables declaration
		//
		int pc, fid, dpc01, dpc02;
		HIGHPRECISION value;
		BINOP bop;
		PRECISION i, j, max_j;
		vector<vector<BlameNodeID>> blameNodeIds;
		vector<bool> edgeAttributes;

		//
		// variables definition
		//
		pc = left.getPC();
		fid = left.getFileID();
		value = left.getValue(precision);
		bop = left.getBinOp();

		//
		// construct a node associate with the binary operation result
		//
		BlameNode node(dpc, pc, fid, false, precision, blameNodeIds, edgeAttributes);

		dpc01 = right01.getDPC();
		dpc02 = right02.getDPC();

		//
		// try different combination of precision of the two operands
		//
		max_j = PRECISION_NO;
		for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
			HIGHPRECISION value01 = right01.getValue(i);
			HIGHPRECISION lowvalue01 = right01.getValue(BITS_FLOAT);

			for (j = BITS_FLOAT; j < max_j; j = PRECISION(j + 1)) {
				HIGHPRECISION value02 = right02.getValue(j);
				HIGHPRECISION lowvalue02 = right02.getValue(BITS_FLOAT);

				safe_assert(value == BlameTreeUtilities::clearBits(value, 52 - BlameTreeUtilities::exactBits(precision)));
				if (BlameTreeUtilities::equalWithPrecision(value, BlameTreeUtilities::eval(value01, value02, bop), precision)) {
					/*
					if (value ==
					BlameTreeUtilities::clearBits(BlameTreeUtilities::eval(value01,
					    value02, bop), 52 - BlameTreeUtilities::exactBits(precision)))
					{*/
					//
					// Construct edges for each blame
					//
					BlameNodeID bnID01(dpc01, i);
					BlameNodeID bnID02(dpc02, j);
					vector<BlameNodeID> blamedNodes;

					//
					// Blame only if the operand cannot be in the lowest precision (right
					// now BITS_23)
					//
					if (i != BITS_FLOAT &&
							BlameTreeUtilities::clearBits(lowvalue01, 52 - BlameTreeUtilities::exactBits(i)) != value01) {
						blamedNodes.push_back(bnID01);
					}

					if (j != BITS_FLOAT &&
							BlameTreeUtilities::clearBits(lowvalue02, 52 - BlameTreeUtilities::exactBits(j)) != value02) {
						blamedNodes.push_back(bnID02);
					}

					node.edges.push_back(blamedNodes);

					//
					// Determine the edge attribute
					//
					node.edgeAttributes.push_back(!BlameTreeUtilities::equalWithPrecision(
													  value, BlameTreeUtilities::feval(value01, value02, bop), precision));
					/*
					node.addEdgeAttribute(BlameTreeUtilities::clearBits(value, 52 -
					      BlameTreeUtilities::exactBits(precision)) !=
					    BlameTreeUtilities::clearBits(BlameTreeUtilities::feval(value01,
					        value02, bop), 52 -
					      BlameTreeUtilities::exactBits(precision)));
					      */

					//
					// Do not try larger j because it subsumes what have been tried
					//
					break;
				}
			}
			//
			// Do not try larger j because it subsumes what have been tried
			//
			max_j = j;
		}

		//
		// Determined whether node is highlighted
		//
		if (value != (LOWPRECISION)value) {
			node.highlight = true;
		}

		nodes[bnID] = node;

		return node;
	}

	return it->second;
}

BlameNode BlameTreeAnalysis::constructBlameGraph(map<int, vector<BlameTreeShadowObject<HIGHPRECISION>>> trace) {

	queue<BlameNodeID> workList;

	workList.push(rootNode);

	while (!workList.empty()) {

		//
		// Variable declarations.
		//
		int dpc;
		PRECISION precision;
		vector<BlameTreeShadowObject<HIGHPRECISION>> startNode;
		set<BlameNodeID> nodeIds;
		BlameNode blameGraph;

		//
		// Variable definitions.
		//
		BlameNodeID bnID = workList.front();
		workList.pop();
		if (nodes.find(bnID) != nodes.end()) {
			continue;
		}

		dpc = bnID.dpc;
		precision = bnID.precision;
		startNode = trace[dpc];

		//
		// We are assuming that each element of the trace has three elements.
		// Construct blameGraph given the start node. Recursively construct
		// blameGraph for all operands that are blamed by the start node.
		//
		switch (startNode[0].getIntrType()) {
			case BIN_INTR:
				safe_assert(startNode.size() == 3);
				blameGraph = constructBlameNode(startNode[0], precision, startNode[1], startNode[2]);
				break;
			case CALL_INTR:
				safe_assert(startNode.size() == 2);
				blameGraph = constructFuncBlameNode(startNode[0], precision, startNode[1]);
				break;
			default:
				DEBUG_STDERR("Unsupport kind of instruction.");
				safe_assert(false);
		}

		const vector<vector<BlameNodeID>>& blameEdges = blameGraph.edges;


		for (auto edgesIt = blameEdges.begin(); edgesIt != blameEdges.end(); ++edgesIt) {
			vector<BlameNodeID> blameNodes;

			blameNodes = *edgesIt;

			for (vector<BlameNodeID>::iterator nodesIt = blameNodes.begin(); nodesIt != blameNodes.end(); ++nodesIt) {
				BlameNodeID blameNode = *nodesIt;

				//
				// Construct graph for this node if it is never constructed before
				//
				nodeIds.insert(blameNode);
			}
		}

		for (set<BlameNodeID>::iterator nodesIt = nodeIds.begin(); nodesIt != nodeIds.end(); ++nodesIt) {
			BlameNodeID blameNode = *nodesIt;
			if (nodes.find(blameNode) == nodes.end()) {
				workList.push(blameNode);
			}
		}
	}

	return nodes[rootNode];
}

std::string BlameTreeAnalysis::edgeToDot(BlameNode graph) {
	std::ostringstream dot;
	std::set<BlameNodeID> bnIDs;

	dot << "\t" << graph.edgeToDot(nodes) << endl;

	return dot.str();
}

std::string BlameTreeAnalysis::toDot() {
	std::ostringstream dot;
	std::set<BlameNodeID> bnIDs;
	vector<vector<BlameNodeID>> edges;
	map<BlameNodeID, BlameNode>::reverse_iterator it;

	// int nodeCnt = 0;

	dot << "digraph G { " << endl;

	for (it = nodes.rbegin(); it != nodes.rend(); ++it) {
		BlameNode bn = it->second;

		if (bn.highlight) {
			dot << "\t" << bn.toDot() << "[color=red]" << endl;
		}

		dot << edgeToDot(bn);

		/*
		cout << nodeCnt << endl;

		nodeCnt++;
		if (nodeCnt > 500) break;
		*/
	}

	dot << "}" << endl;

	return dot.str();
}

void BlameTreeAnalysis::printResult() {
	queue<BlameNodeID> workList;
	set<BlameNodeID> cacheNodes;  // set of already considered blame nodes
	map<pair<int, int>, vector<bool>> result;  // map from pc to a pair of boolean, the first
	// boolean indicates whether the result
	// requires higherprecision, the second
	// boolean indicates whether the operator
	// requires higher precision
	workList.push(rootNode);
	cacheNodes.insert(rootNode);

	while (!workList.empty()) {
		int pc;
		int file;

		bool highlight;
		bool edgeHighlight;

		BlameNodeID bnID = workList.front();
		BlameNode bn = nodes[bnID];
		workList.pop();
		pc = bn.dpc;
		file = bn.fid;

		highlight = bn.highlight;
		const vector<vector<BlameNodeID>>& edges = bn.edges;
		const vector<bool>& edgeAttributes = bn.edgeAttributes;

		if (!edges.empty()) {
			vector<BlameNodeID> bnIDs;

			edgeHighlight = edgeAttributes[0];
			//
			// save the result for the current node
			//
			if (result.find(make_pair(file, pc)) == result.end()) {
				vector<bool> nodeResult;

				nodeResult.push_back(highlight);
				nodeResult.push_back(edgeHighlight);
				result[make_pair(file, pc)] = nodeResult;
			} else {
				vector<bool> nodeResult;

				nodeResult = result[make_pair(file, pc)];
				nodeResult[0] = nodeResult[0] || highlight;
				nodeResult[1] = nodeResult[1] || edgeHighlight;
				result[make_pair(file, pc)] = nodeResult;
			}

			bnIDs = edges[0];
			for (vector<BlameNodeID>::iterator it = bnIDs.begin(); it != bnIDs.end(); ++it) {
				BlameNodeID bnID = *it;
				if (cacheNodes.find(bnID) == cacheNodes.end()) {
					cacheNodes.insert(bnID);
					workList.push(bnID);
				}
			}
		}
	}

	//
	// print result
	//
	for (map<pair<int, int>, vector<bool>>::iterator it = result.begin(); it != result.end(); it++) {
		int pc, file;
		bool highlight, edgeHighlight;

		file = it->first.first;
		pc = it->first.second;
		highlight = it->second[0];
		edgeHighlight = it->second[1];

		if (highlight || edgeHighlight) {
			cout << "\t File: " << file << ", Line " << pc << ":" << endl;
			if (highlight) {
				cout << "\t\t Result: double precision" << endl;
			}
			if (edgeHighlight) {
				cout << "\t\t Operator: double precision" << endl;
			}
		}
	}
}
