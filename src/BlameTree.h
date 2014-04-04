/**
 * @file BlameTree.h
 * @brief BlameTree Declarations.
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

// Author: Cuong Nguyen and Cindy Rubio-Gonzalez

#ifndef BLAME_TREE_H_
#define BLAME_TREE_H_

#include "InterpreterObserver.h"
#include "BlameTreeShadowObject.h"
#include "IValue.h"
#include <math.h>

using namespace llvm;
using namespace std;

class BlameTree : public InterpreterObserver {
  
  public:
    typedef double HIGHPRECISION;
    typedef float LOWPRECISION;
    typedef enum {BITS_23, BITS_30, BITS_37, BITS_44, BITS_52} PRECISION;

    static int outputPC;    // location of the output value to track from
    static int dynamicCounter; // unique counter for instructions executed
    static HIGHPRECISION errorThreshold;    // the allowable error threshold  on the output
    static HIGHPRECISION machineEpsilon;    // the machine smallest unit in high precision
    static map<int, BlameTreeShadowObject<HIGHPRECISION> > trace;

    BlameTree(std::string name) : InterpreterObserver(name) {}

    virtual void pre_allocax(IID iid, KIND kind, uint64_t size, int inx, 
			   int line, bool arg, KVALUE* result);

    virtual void post_allocax(IID iid, KIND kind, uint64_t size, int inx, 
			      int line, bool arg, KVALUE* result);

    virtual void pre_load(IID iid, KIND kind, SCOPE opScope, int opInx, uint64_t 
			  opAddr, bool loadGlobal, int loadInx, int file, int line, int in);
    
    virtual void post_load(IID iid, KIND kind, SCOPE opScope, int opInx, uint64_t 
			   opAddr, bool loadGlobal, int loadInx, int file, int line, int in);

    virtual void pre_store(int pInx, SCOPE pScope, KIND srcKind, SCOPE srcScope, int srcInx, int64_t 
			 srcValue, int file, int line, int inx);

    virtual void post_store(int pInx, SCOPE pScope, KIND srcKind, SCOPE srcScope, int srcInx, int64_t 
			    srcValue, int file, int line, int inx);

    virtual void pre_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t
        rValue, KIND type, int line, int inx);

    virtual void post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue,
        int64_t rValue, KIND type, int line, int inx);

    virtual void pre_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t
        rValue, KIND type, int line, int inx);

    virtual void post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue,
        int64_t rValue, KIND type, int line, int inx);

    virtual void pre_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t
        rValue, KIND type, int line, int inx);

    virtual void post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue,
        int64_t rValue, KIND type, int line, int inx);

    virtual void pre_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t
        rValue, KIND type, int line, int inx);

    virtual void post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue,
        int64_t rValue, KIND type, int line, int inx);

    //virtual void post_create_global_symbol_table();

    virtual void post_analysis();

  private:
    BlameTreeShadowObject<HIGHPRECISION> preBtmSO;

    /**
     * Define how to copy BlameTreeShadowObject from the source IValue to
     * the destination IValue.
     */
    static void copyShadow(IValue *src, IValue *dest);

    /**
     * Return BlameTreeShadowObject associated with the given value.
     *
     * @note a value is denoted by a pair of scope and const/index.
     * @param scope scope of the value.
     * @param constOrIndex constant/index of the value.
     * @return BlameTreeShadowObject associated with the given value.
     */
    HIGHPRECISION getShadowValue(SCOPE scope, int64_t constOrIndex, PRECISION precision);

    /**
     * Return the actual value in its lower precision.
     *
     * @note a value is denoted by a pair of scope and const/index.
     * @param scope scope of the value.
     * @param constOrIndex constant/index of the value.
     * @return the actual value in its lower precision.
     */
    LOWPRECISION getActualValue(SCOPE scope, int64_t constOrIndex);

    /**
     * Return the program counter associated with the given value.
     *
     * @note a value is denoted by a pair of scope and const/index.
     * @param scope scope of the value.
     * @param constOrIndex constant/index of the value.
     * @return the program counter associated with the given value.
     */
    int getPC(SCOPE scope, int64_t value);

    HIGHPRECISION computeRelativeError(HIGHPRECISION highValue, LOWPRECISION lowValue);

    void pre_fpbinop(int inx);

    void post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t
        rValue, KIND type, int line, int inx, BINOP op);
};

#endif
