// Author: Cuong Nguyen

#ifndef BLAME_NODE_H
#define BLAME_NODE_H

#include "BlameNodeID.h"
#include "BlameTreeUtilities.h"

/**
 * BlameNode is abstraction of a dynamic instruction in the program.
 *
 * We assume that the program is in three-address-code, therefore,
 * each BlameNode is also associated with a value of the program. Each
 * BlameNode has a unique dynamic program counter, a (not neccessarily
 * unique) source code program counter, and a precision constraint associated
 * with the
 * value it carries. Each blame node can blame (connect to) a set of node group
 * that decides its precision constraint.
 */
class BlameNode {
private:
	int dpc;        // Dynamic program counter of instruction
	// associated with this blame tree node.
	int pc;         // Source program counter of instruction
	// associated with this blame tree node.
	int fid;        // Id of source file containing instruction
	// associated with this blame tree node.
	bool highlight; // Highlighted node indicates higher precision requirement.
	PRECISION precision; // The precision constraint of this blame tree node,
	// determine up to how many digits the value of this
	// node needs to be accurate.
	vector<vector<BlameNodeID>> edges; // Set of nodes that this
	// node blames, a node is identified
	// by a pair of dpc and precision.
	vector<bool> edgeAttributes;        // Set of edge attributes
	// right now it indicates whether
	// the edge (the computation)
	// requires high precision.

public:

	BlameNode()
		: dpc(0), pc(0), fid(0), highlight(false), precision(BITS_FLOAT) {}
	;

	BlameNode(int dp, int p, int f, bool hl, PRECISION prec,
			  vector<vector<BlameNodeID>> es, vector<bool> eas)
		: dpc(dp), pc(p), fid(f), highlight(hl), precision(prec), edges(es),
		  edgeAttributes(eas) {}
	;

	BlameNode(const BlameNode& btNode) {
		create(btNode);
	}
	;

	~BlameNode() {
		uncreate();
	}
	;

	BlameNode& operator=(const BlameNode& btNode) {
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

	bool isHighlight() const {
		return highlight;
	}
	;

	void setHighlight(bool highlight) {
		this->highlight = highlight;
	}
	;

	PRECISION getPrecision() const {
		return precision;
	}
	;

	void setPrecision(PRECISION precision) {
		this->precision = precision;
	}
	;

	vector<vector<BlameNodeID>> getEdges() const {
		return edges;
	}
	;

	void setEdges(vector<vector<BlameNode>>) {
		this->edges = edges;
	}
	;

	void addBlamedNodes(vector<BlameNodeID> nodes) {
		edges.push_back(nodes);
	}

	vector<bool> getEdgeAttributes() const {
		return edgeAttributes;
	}
	;

	void setEdgeAttributes(vector<bool> edgeAttributes) {
		this->edgeAttributes = edgeAttributes;
	}
	;

	void addEdgeAttribute(bool ea) {
		edgeAttributes.push_back(ea);
	}
	;

	/**
	 * Visualize this node in GraphViz dot format.
	 */
	std::string toDot();

	/**
	 * Visualize edges associate with this ndoe in GraphViz dot format.
	 */
	std::string edgeToDot(map<BlameNodeID, BlameNode> nodes);

private:
	void create(const BlameNode& btNode) {
		dpc = btNode.getDPC();
		pc = btNode.getPC();
		fid = btNode.getFileID();
		highlight = btNode.isHighlight();
		precision = btNode.getPrecision();
		edges = btNode.getEdges();
		edgeAttributes = btNode.getEdgeAttributes();
	}
	;

	void uncreate() {}
	;
};
#endif
