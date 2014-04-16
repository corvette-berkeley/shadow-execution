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
#include "BlameTreeUtilities.h"
#include <limits>
#include <sstream>

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

template <class T>
class BlameTreeShadowObject {

  private:
    int pc;                 // Program counter of the instruction associate with this object
    int dpc;                // Program counter of the instruction as appeared in the execution trace
    int fid;                // Id of the file containing this instruction
    INTRTYPE intrType;      // Type of the instruction
    BINOP binOp;            // Binary operator (if instruction is BINOP) 
    string func;            // Name of the function call (if instruction is CALL)
    T value[PRECISION_NO];             // Value in 5 different precision

  public:
    BlameTreeShadowObject(): pc(0), dpc(0), intrType(INTRTYPE_INVALID),
    binOp(BINOP_INVALID), func("NONE") {
      PRECISION i;
      for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i+1)) {
        value[i] = 0;
      }
    };

    BlameTreeShadowObject(int p, int dp, INTRTYPE it, BINOP bo, string f, T *val): pc(p),
    dpc(dp), intrType(it), binOp(bo), func(f) {
      PRECISION i;
      for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i+1)) {
        value[i] = val[i];
      }
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

    string getFunc() const { return func; };

    void setFunc(string func) { this->func = func; };

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
      PRECISION i;

      cout << "[SHADOW]<pc: " << pc << ", dpc: " << dpc;
      cout.precision(20);
//      cout.precision(std::numeric_limits< double >::digits10);
      for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i+1)) {
        cout << ", " << BlameTreeUtilities::precisionToString(i) << ":" << value[i];
      }
      cout << ", op: " << BINOP_ToString(binOp).c_str() << ", func:" << func.c_str() << endl;
    }

  private:
    void create(const BlameTreeShadowObject& btmSO) {
      PRECISION i;

      pc = btmSO.getPC();
      dpc = btmSO.getDPC();
      fid = btmSO.getFileID();
      intrType = btmSO.getIntrType();
      binOp = btmSO.getBinOp();
      func = btmSO.getFunc();
      for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i+1)) {
        value[i] = btmSO.getValue(i);
      }
    };

    void uncreate() {};
};

#endif
