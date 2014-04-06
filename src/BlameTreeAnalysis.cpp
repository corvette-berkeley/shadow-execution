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

BlameNode BlameTreeAnalysis::constructBlameNode(BlameTreeShadowObject<HIGHPRECISION>
    left, PRECISION precision,
    BlameTreeShadowObject<HIGHPRECISION> right01,
    BlameTreeShadowObject<HIGHPRECISION> right02) {

  //
  // variables declaration
  //
  int dpc, pc, fid, dpc01, dpc02;
  std::map<BlameNodeID, BlameNode>::iterator it;
  HIGHPRECISION value;
  BINOP bop;

  //
  // variables definition
  //
  dpc = left.getDPC();
  BlameNodeID bnID(dpc, precision);
  pc = left.getPC(); 
  fid = left.getFileID();
  value = left.getValue(precision);
  bop = left.getBinOp();
  it = nodes.find(bnID);

  //
  // nodes remember all nodes that are processed before
  // process again only if this is not processed before
  //
  if (it != nodes.end()) {
    PRECISION i, j, max_j;
    vector< vector< BlameNodeID > > blameNodeIds;

    //
    // construct a node associate with the binary operation result
    //
    BlameNode node(dpc, pc, fid, false, precision, blameNodeIds);

    dpc01 = right01.getDPC();
    dpc02 = right02.getDPC();

    //
    // try different combination of precision of the two operands
    //
    max_j = PRECISION_NO;
    for (i = BITS_23; i < PRECISION_NO; i =
        PRECISION(i+1)) {
      HIGHPRECISION value01 = right01.getValue(i);
      HIGHPRECISION lowvalue01 = right01.getValue(BITS_23);

      for (j = BITS_23; j < max_j; j = PRECISION(j+1)) {
        HIGHPRECISION value02 = right02.getValue(j);
        HIGHPRECISION lowvalue02 = right02.getValue(BITS_23);

        if (BlameTreeUtilities::clearBits(value, precision) ==
            BlameTreeUtilities::clearBits(BlameTreeUtilities::eval(value01,
                value02, bop), precision)) {
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
          if (BlameTreeUtilities::clearBits(lowvalue01, i) != value01) 
            blamedNodes.push_back(bnID01);
          if (BlameTreeUtilities::clearBits(lowvalue02, j) != value02) 
            blamedNodes.push_back(bnID02);
          node.addBlamedNodes(blamedNodes);

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
    if (value != (LOWPRECISION) value)
      node.setHighlight(true);

    nodes[bnID] = node;

    return node;
  } 

  return it->second;
}

BlameNode BlameTreeAnalysis::constructBlameGraph(map<int,
    vector<BlameTreeShadowObject<HIGHPRECISION> > > trace,
    BlameNodeID bnID) {
  //
  // Variable declarations.
  //
  int dpc; 
  PRECISION precision;
  vector< BlameTreeShadowObject<HIGHPRECISION> > startNode;
  vector< vector< BlameNodeID > > blameEdges;
  BlameNode blameGraph;

  //
  // Variable definitions.
  //
  dpc = bnID.getDPC();
  precision = bnID.getPrecision();
  startNode = trace[dpc];

  //
  // We are assuming that each element of the trace has three elements.
  // Construct blameGraph given the start node. Recursively construct
  // blameGraph for all operands that are blamed by the start node.
  //
  safe_assert(startNode.size() == 3);
  blameGraph = constructBlameNode(startNode[0], precision, startNode[1], startNode[2]);
  blameEdges = blameGraph.getEdges();

  for (vector< vector< BlameNodeID > >::iterator edgesIt = blameEdges.begin(); edgesIt != blameEdges.end(); ++edgesIt) {
    vector< BlameNodeID > blameNodes;

    blameNodes = *edgesIt;

    for (vector<BlameNodeID>::iterator nodesIt = blameNodes.begin(); nodesIt != blameNodes.end(); ++nodesIt) {
      BlameNodeID blameNode = *nodesIt;

      //
      // Construct graph for this node if it is never constructed before
      //
      if (nodes.find(blameNode) != nodes.end()) {
        constructBlameGraph(trace, blameNode);
      }
    }
  } 

  return blameGraph;
}

std::string BlameTreeAnalysis::edgeToDot(BlameNode graph) {
  std::ostringstream dot;
  std::set<BlameNodeID> bnIDs;
  vector< vector< BlameNodeID > > edges;

  edges = graph.getEdges();

  dot << "\t" << graph.edgeToDot(nodes) << endl;
  for (vector< vector< BlameNodeID > >::iterator edgesIt = edges.begin();
      edgesIt != edges.end(); ++edgesIt) {

    vector< BlameNodeID > blameNodes = *edgesIt;

    for (vector< BlameNodeID >::iterator nodesIt = blameNodes.begin(); nodesIt
        != blameNodes.end(); ++nodesIt) {
      BlameNodeID bnID = *nodesIt;

      if (bnIDs.find(bnID) != bnIDs.end()) {
        BlameNode bn = nodes[bnID];
        dot << edgeToDot(bn);

        bnIDs.insert(bnID);
      }
    }
  }

  return dot.str();
}

std::string BlameTreeAnalysis::toDot(BlameNode graph) {
  std::ostringstream dot;
  std::set<BlameNodeID> bnIDs;
  vector< vector< BlameNodeID > > edges;

  dot << "digraph G { " << endl;

  dot << edgeToDot(graph);

  dot << "}" << endl;

  return dot.str();
}
