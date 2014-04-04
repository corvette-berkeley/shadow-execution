/**
 * @file BlameTreeShadowObject.h
 * @brief BlameTreeShadowObject Declarations.
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

#ifndef BLAME_TREE_SHADOW_OBJECT_H
#define BLAME_TREE_SHADOW_OBJECT_H

#include "Constants.h"

/**
 * BlameTreeShadowObject is an abstraction of a value in the program in
 * higher precision. 
 *
 * We make an assumption that each value corresponds to an instruction in the
 * program (similar to the way LLVM program constructed). Therefore, each
 * BlameTreeShadowObject also has attributes like intruction type, program
 * location, etc.
 *
 * Each BlameTreeShadowObject is more informative than the actual value in
 * higher precision because it consists of other information for error
 * analysis, e.g. maximum relative errors, sources of maximum relative errors,
 * etc.
 */

typedef enum {
  BIN_INTR, UNARY_INTR, CONSTANT_INTR, CALL_INTR, LOAD_INTR, STORE_INTR,
  INTRTYPE_INVALID
} INTRTYPE;

template <class T>
class BlameTreeShadowObject {

  private:
    int pc;                 // Program counter of the instruction associate with this object
    int dpc;                // Program counter of the instruction as appeared in the execution trace
    int fid;                // Id of the file containing this instruction
    INTRTYPE intrType;      // Type of the instruction
    BINOP binOp;            // Binary operator (if instruction is BINOP) 
    T value[5];   // Value in 5 different precision

  public:
    BlameTreeShadowObject(): pc(0), dpc(0), intrType(INTRTYPE_INVALID),
    binOp(BINOP_INVALID) {
      value[0] = 0; 
      value[1] = 1;
      value[2] = 2;
      value[3] = 3;
      value[4] = 4;
    };

    BlameTreeShadowObject(int p, int dp, INTRTYPE it, BINOP bo, T *val): pc(p),
    dpc(dp), intrType(it), binOp(bo) {
      value[0] = val[0];
      value[1] = val[1];
      value[2] = val[2];
      value[3] = val[3];
      value[4] = val[4];
    } 
      
    BlameTreeShadowObject(const BlameTreeShadowObject& btmSO) {
      create(btmSO);
    };

    ~BlameTreeShadowObject() {
      uncreate();
    }

    BlameTreeShadowObject& operator=(const BlameTreeShadowObject& btmSO) {
      if (&btmSO != this) {
        
        // free the object in the left-hand side
        uncreate();

        // copy elements from the righ-hand side to the left-hand side
        create(btmSO);
      }

      return *this;
    };

    int getPC() const { return pc; };

    void setPC(int pc) { this->pc = pc; };

    int getDPC() const { return dpc; };

    void setDPC(int dpc) { this->dpc = dpc; };

    int getFileID() const { return fid; };

    void setFileID(int fileID) { this->fid = fileID; };

    INTRTYPE getIntrType() const { return intrType; };

    void setIntrType(INTRTYPE intrType) { this->intrType = intrType; };

    BINOP getBinOp() const { return binOp; };

    void setBinOp(BINOP binOp) { this->binOp = binOp; };

    T getValue(int i) const { return value[i]; };

    void setValue(int i, T v) { value[i] = v; };

    std::string BINOP_ToString(int binop) {
      std::stringstream s;
      switch(binop) {
      case ADD:
	s << "ADD";
	break;
      case FADD:
	s << "FADD";
	break;
      case SUB:
	s << "SUB";
	break;
      case FSUB:
	s << "FSUB";
	break;
      case MUL:
	s << "MUL";
	break;
      case FMUL:
	s << "FMUL";
	break;
      case UDIV:
	s << "UDIV";
	break;
      case SDIV:
	s << "SDIV";
	break;
      case FDIV:
	s << "FDIV";
	break;
      case UREM:
	s << "UREM";
	break;
      case SREM:
	s << "SREM";
	break;
      case FREM:
	s << "FREM";
	break;
      default: 
	s << "INVALID";
	break;
      }
      return s.str();
    };

    void print() {
      printf("[SHADOW]<pc: %d, dpc: %d, value: %f, op: %s>\n", pc, dpc, value[4], BINOP_ToString(binOp).c_str());
    }

  private:
    void create(const BlameTreeShadowObject& btmSO) {
      pc = btmSO.getPC();
      dpc = btmSO.getDPC();
      fid = btmSO.getFileID();
      intrType = btmSO.getIntrType();
      binOp = btmSO.getBinOp();
      value[0] = btmSO.getValue(0);
      value[1] = btmSO.getValue(1);
      value[2] = btmSO.getValue(2);
      value[3] = btmSO.getValue(3);
      value[4] = btmSO.getValue(4);
    };

    void uncreate() {};
};

#endif
