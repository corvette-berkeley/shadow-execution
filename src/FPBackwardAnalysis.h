/**
 * @file FPBackwardAnalysis.h
 * @brief FPBackwardAnalysis Declarations.
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
 * 3. All advertising materials mentioning features or use of this software must
 * display the following acknowledgement: This product includes software
 * developed by the UC Berkeley.
 *
 * 4. Neither the name of the UC Berkeley nor the names of its contributors may
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

// Author: Cindy Rubio-Gonzalez

#ifndef FP_BACKWARD_ANALYSIS_H_
#define FP_BACKWARD_ANALYSIS_H_

#include "InterpreterObserver.h"
#include "FPBackwardShadowObject.h"
#include "IValue.h"
#include <map>

using namespace llvm;
using namespace std;

class FPBackwardAnalysis : public InterpreterObserver {

  public:
    static int source; // source of code location to track from
    static double epsilon; // the error threshold to track

    typedef long double PRECISION;

    FPBackwardAnalysis(std::string name) : InterpreterObserver(name) {}

    virtual void pre_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);
    
    virtual void pre_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void pre_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void pre_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void pre_fcmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, PRED pred, int line, int inx);

    //virtual void post_fcmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, PRED pred, int line, int inx);

    virtual void pre_fptoui(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    //virtual void post_fptoui(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    virtual void pre_fptosi(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    //virtual void post_fptosi(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    virtual void pre_fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    //virtual void post_fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    virtual void pre_fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    //virtual void post_fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

    //virtual void pre_sync_call(int callerId, int line);

    //virtual void post_sync_call(int callerId, int line);

    //virtual void post_create_global_symbol_table();

    //virtual void post_analysis();
    

  private:
    map<int, FPBackwardShadowObject > analysisTable;  // store analysis information
    FPBackwardShadowObject *preFpISO;                  // store the FPBackwardShadowObject
                                                                  // before the operation happens
                                         

    /**
     * Copy shadow value (long double value) from the source IValue to the
     * destination IValue.
     */
    static void copyShadow(IValue *src, IValue *dest);

    /**
     * Return long double shadow value for the given value.
     *
     * @note a value is denoted by a pair of scope and value/index.
     * @param scope scope of the value
     * @param value value/index of the value
     * @return long double shadow value.
     */
    long double getShadowValue(SCOPE scope, int64_t value);

    /**
     * Return concrete value for the given value.
     *
     * @note a value is denoted by a pair of scope and value/index.
     * @param scope scope of the value
     * @param value value/index of the value
     * @return concrete value casted to long double.
     */
    long double getConcreteValue(SCOPE scope, int64_t value);

    /**
     * Return line number for the given value.
     *
     * @note a value is denoted by a pair of scope and value/index.
     * @param scope scope of the value
     * @param value value/index of the value
     * @return line number of the given value
     */
    int getLineNumber(SCOPE scope, int64_t value);

    /**
     * Get the FPBackwardShadowObject before the operation happens.
     */
    void pre_analysis(int inx);

    /**
     * Analysis code after floating-point binary operation. 
     */
    void post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx, BINOP op);
};

#endif /* FP_BACKWARD_ANALYSIS_H_ */

