// Author: Cuong Nguyen

#ifndef BLAME_TREE_ANALYSIS_H
#define BLAME_TREE_ANALYSIS_H

#include "BlameNode.h"
#include "BlameTreeShadowObject.h"
#include "BlameTreeUtilities.h"
#include <queue>

class BlameTree {
private:
  map<BlameNodeID, BlameNode> nodes; // map from a pair (node id, precision) to
                                     // node set of nodes in the tree
	BlameNodeID rootNode; // root node of the tree

	/**
	 * Construct blame node given a binary operation expression. This function
	 * connects the node of left to the node of right01 or right02 if it can
	 * blame right01 or right02 given the precision constraint.
	 *
	 * @param left the left value of the binary operation
	 * @param precision the precision constraint on left
	 * @param right01 the first operand of the binary operation
	 * @param right02 the second oeprand of the binary opeartion
	 *
	 * @return the blame node associated with left and precision constraint,
	 * that connects to nodes it blames
	 */
	BlameNode constructBlameNode(BlameTreeShadowObject<HIGHPRECISION> left,
								 PRECISION precision,
								 BlameTreeShadowObject<HIGHPRECISION> right01,
								 BlameTreeShadowObject<HIGHPRECISION> right02);

	BlameNode constructFuncBlameNode(BlameTreeShadowObject<HIGHPRECISION> left,
									 PRECISION precision,
									 BlameTreeShadowObject<HIGHPRECISION> right);

	BlameNode constructTruncBlameNode(BlameTreeShadowObject<HIGHPRECISION> left,
									  PRECISION precision,
									  BlameTreeShadowObject<HIGHPRECISION> right);

	BlameNode constructExtBlameNode(BlameTreeShadowObject<HIGHPRECISION> left,
									PRECISION precision,
									BlameTreeShadowObject<HIGHPRECISION> right);

	/**
	 * Helper function for toDot function. This function visualize all edges of
	 * the graph in GraphViz dot format.
	 *
	 * @return dot visualization of an edge
	 */
	std::string edgeToDot(BlameNode graph);

public:

	BlameTree(BlameNodeID bnID) : rootNode(bnID) {}
	;

	map<BlameNodeID, BlameNode> getNodes() {
		return nodes;
	}
	;

	/**
	 * Output the results for each lines of code, including whether the result
	 * needs higher precision or the operator needs higher precision.
	 */
	void printResult();

	/**
	 * Visualize blame graph in GraphViz dot format.
	 *
	 * @return dot program of the visualized blame graph
	 */
	std::string toDot();

	/**
	 * Construct the blame graph given the execution trace and a node to start
	 * with. The constructed graph contains all the nodes that the start node
	 * can blame given its precision constraint.
	 *
	 * @param trace the program execution trace
	 * @return the blame graph
	 */
	BlameNode constructBlameGraph(
		vector<vector<BlameTreeShadowObject<HIGHPRECISION> > > trace);
};

#endif
