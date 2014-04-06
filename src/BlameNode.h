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
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen

#ifndef BLAME_NODE_H
#define BLAME_NODE_H

#include "BlameTree.h"
#include "BlameNodeID.h"
#include "BlameTreeUtilities.h"

/**
 * BlameNode is abstraction of a dynamic instruction in the program. 
 *
 * We make an assumption that the program is in three-address-code, therefore,
 * each BlameNode is also associated with a value of the program. Each
 * BlameNode has a unique dynamic program counter, a (not neccessarily
 * unique) source code program counter, and a precision constraint associated with the
 * value it carries. Each blame node can blame (connect to) a set of node group
 * that decides its precision constraint.
 */
class BlameNode {
  private:
    int dpc;        // dynamic program counter of instruction associated with this blame tree node
    int pc;         // source program counter of instruction associated with this blame tree noe
    int fid;        // id of source file containing instruction associated with this blame tree node
    BlameTree::PRECISION precision;    // the precision constraint of this blame tree node
    vector< vector< BlameNodeID > > edges;    // set of nodes that this
                                                     // node blames, a node is identified 
                                                     // by a pair of dpc and precision

  public:

    BlameNode(): dpc(0), pc(0), fid(0), precision(BlameTree::BITS_23) {};

    BlameNode(int dp, int p, int f, BlameTree::PRECISION prec, vector< vector<
        BlameNodeID > > es): dpc(dp), pc(p), fid(f), precision(prec), edges(es) {};

    BlameNode(const BlameNode& btNode) {
      create(btNode);
    };

    ~BlameNode() {
      uncreate();
    };

    BlameNode& operator=(const BlameNode& btNode) {
      if (&btNode != this) {

        // free the object in the left-hand side
        uncreate();

        // copy elements from the right-hand side to the left-hand side
        create(btNode);
      }

      return *this;
    };

    int getDPC() const { return dpc; };

    void setDPC(int dpc) { this->dpc = dpc; };

    int getPC() const { return pc; };

    void setPC(int pc) { this->pc = pc; };

    int getFileID() const { return fid; };

    void setFileID(int fid) { this->fid = fid; };

    BlameTree::PRECISION getPrecision() const { return precision; };

    void setPrecision(BlameTree::PRECISION precision) { this->precision = precision; };

    vector< vector< BlameNodeID > > getEdges() const { return edges; };

    void setEdges(vector < vector< BlameNode > >) { this->edges = edges; };

    void addBlamedNodes(vector< BlameNodeID > nodes) { edges.push_back(nodes); }

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
      precision = btNode.getPrecision();
      edges = btNode.getEdges();
    };

    void uncreate() {};
};
#endif
