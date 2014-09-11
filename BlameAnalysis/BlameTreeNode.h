// Author: Cuong Nguyen

#ifndef BLAME_TREE_NODE_H
#define BLAME_TREE_NODE_H

#include "BlameTree.h"

/**
 * BlameTreeNode is abstraction of a dynamic instruction in the program.
 *
 * We make an assumption that the program is in three-address-code, therefore,
 * each BlameTreeNode is also associated with a value of the program. Each
 * BlameTreeNode has a unique dynamic program counter, a (not neccessarily
 * unique) source code program counter, and a precision constraint associated
 * with the value it carries. Each blame node can blame (connect to) a set of
 * node groupthat decides its precision constraint.
 */
class BlameTreeNode {
private:
	int dpc; // Dynamic program counter of instruction associated with
	// this blame tree node.
	int pc;  // Source program counter of instruction associated with
	// this blame tree node.
	int fid; // Id of source file containing instruction associated with
	// this blame tree node
	BlameTree::PRECISION precision;       // The precision constraint of this
	// blame tree node.
	vector<vector<BlameTreeNode>> edges; // Set of nodes that this node
	// blames.

public:

	BlameTreeNode() : dpc(0), pc(0), fid(0), precision(BlameTree::BITS_23) {}
	;

	BlameTreeNode(int dp, int p, int f, BlameTree::PRECISION prec,
				  vector<vector<BlameTreeNode>> es)
		: dpc(dp), pc(p), fid(f), precision(prec), edges(es) {}
	;

	BlameTreeNode(const BlameTreeNode& btNode) {
		create(btNode);
	}
	;

	~BlameTreeNode() {
		uncreate();
	}
	;

	BlameTreeNode& operator=(const BlameTreeNode& btNode) {
		if (&btNode != this) {

			// free the object in the left-hand side
			uncreate();

			// copy elements from the right-hand side to the left-hand side
			create(btNode);
		}

		return *this;
	}
	;

	int getDPC() const {
		return dpc;
	}
	;

	void setDPC(int dpc) {
		this->dpc = dpc;
	}
	;

	int getPC() const {
		return pc;
	}
	;

	void setPC(int pc) {
		this->pc = pc;
	}
	;

	int getFileID() const {
		return fid;
	}
	;

	void setFileID(int fid) {
		this->fid = fid;
	}
	;

	BlameTree::PRECISION getPrecision() const {
		return precision;
	}
	;

	void setPrecision(BlameTree::PRECISION precision) {
		this->precision = precision;
	}
	;

	vector<vector<BlameTreeNode>> getEdges() const {
		return edges;
	}
	;

	void setEdges(vector<vector<BlameTreeNode>>) {
		this->edges = edges;
	}
	;

	void addNodes(vector<BlameTreeNode> nodes) {
		edges.push_back(nodes);
	}

private:
	void create(const BlameTreeNode& btNode) {
		dpc = btNode.getDPC();
		pc = btNode.getPC();
		fid = btNode.getFileID();
		precision = btNode.getPrecision();
		edges = btNode.getEdges();
	}
	;

	void uncreate() {}
	;
};
#endif
