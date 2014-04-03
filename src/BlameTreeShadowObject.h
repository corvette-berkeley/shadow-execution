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
template <class T>
class BlameTreeShadowObject {
  public:
    typedef enum {
      BIN_INTR, UNARY_INTR, CONSTANT_INTR, CALL_INTR, LOAD_INTR, STORE_INTR,
      INTRTYPE_INVALID
    } INTRTYPE;

  private:
    int pc;                 // Program counter of the instruction associate with this object
    int dpc;                // Program counter of the instruction as appeared in the execution trace
    INTRTYPE intrType;      // Type of the instruction
    BINOP binOp;            // Binary operator (if instruction is BINOP) 
    T[5] value;             // Value in 5 different precision

  public:
    BlameTreeShadowObject(): pc(0), dpc(0)
      
      
      value(0), intrType(INTRTYPE_INVALID),
        binOp(BINOP_INVALID), td(1), pc(0), fid(0), mre(0), mreLowValue(0),
        mreHighValue(0) {
      mreSrc[0] = -1;
      mreSrc[1] = -1;
      mreSrcLowValue[0] = 0;
      mreSrcLowValue[1] = 0;
      mreSrcHighValue[0] = 0;
      mreSrcHighValue[1] = 0;
    };

    BlameTreeShadowObject(T v, int l, INTRTYPE it, BINOP bo): value(v),
        intrType(it), binOp(bo), td(1), pc(l), fid(0), mre(0), mreLowValue(0),
        mreHighValue(0) {
      mreSrc[0] = -1;
      mreSrc[1] = -1;
      mreSrcLowValue[0] = 0;
      mreSrcLowValue[1] = 0;
      mreSrcHighValue[0] = 0;
      mreSrcHighValue[1] = 0;
    };

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

    T getValue() const { return value; };

    void setValue(T value) { this->value = value; };

    INTRTYPE getIntrType() const { return intrType; };

    void setIntrType(INTRTYPE intrType) { this->intrType = intrType; };

    BINOP getBinOp() const { return binOp; };

    void setBinOp(BINOP binOp) { this->binOp = binOp; };

    int getComputationDepth() const { return td; };

    void setComputationDepth(int td) { this->td = td; };

    int getPC() const { return pc; };

    void setPC(int pc) { this->pc = pc; };

    int getFileID() const { return fid; };

    void setFileID(int fileID) { this->fileID = fileID; };

    T getMaxRelErr() const { return mre; };

    void setMaxRelErr(T mre) { this->mre = mre; };

    T getMaxRelErrLowValue() const { return mreLowValue; };

    void setMaxRelErrLowValue(T mreLowValue) { this->mreLowValue = mreLowValue; };

    T getMaxRelErrHighValue() const { return mreHighValue; };

    void setMaxRelErrHighValue(T mreHighValue) { this->mreHighValue = mreHighValue; };

    int getMaxRelErrSource(int i) const {
      safe_assert(i == 0 || i == 1);

      return mreSrc[i];
    };

    void setMaxRelErrSource(int i, int pc) {
      safe_assert(i == 0 || i == 1);

      mreSrc[i] = pc;
    };

    T getMaxRelErrSrcLowValue(int i) const {
      safe_assert(i == 0 || i == 1);

      return mreSrcLowValue[i];
    }

    void setMaxRelErrSrcLowValue(int i , T value) {
      safe_assert(i == 0 || i == 1);

      return mreSrcLowValue[i];
    }

    T getMaxRelErrSrcHighValue(int i) const {
      safe_assert(i == 0 || i == 1);

      return mreSrcHighValue[i];
    };

    void setMaxRelErrSrcHighValue(int i, T value) {
      safe_assert(i == 0 || i == 1);
      
      mreSrcHighValue[i] = value;
    };

  private:
    void create(const BlameTreeShadowObject& btmSO) {
      value = btmSO.getValue();
      intrType = btmSO.getIntrType();
      binOp = btmSO.getBinOp();
      td = btmSO.getComputationDepth();
      pc = btmSO.getPC();
      fid = btmSO.getFileID();
      mre = btmSO.getMaxRelErr();
      mreLowValue = btmSO.getMaxRelErrLowValue();
      mreHighValue = btmSO.getMaxRelErrHighValue();
      mreSrc[0] = btmSO.getMaxRelErrSource(0);
      mreSrc[1] = btmSO.getMaxRelErrSource(1);
      mreSrcLowValue[0] = btmSO.getMaxRelErrSrcLowValue(0);
      mreSrcLowValue[1] = btmSO.getMaxRelErrSrcLowValue(1);
      mreSrcHighValue[0] = btmSO.getMaxRelErrSrcHighValue(0);
      mreSrcHighValue[1] = btmSO.getMaxRelErrSrcHighValue(1);
    };

    void uncreate() {};
};

#endif
