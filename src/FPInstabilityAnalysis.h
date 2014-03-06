/**
 * @file FPInstabilityAnalysis.h
 * @brief FPInstabilityAnalysis Declarations.
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

// Author: Cuong Nguyen

#ifndef FP_INSTABILITY_ANALYSIS_H_
#define FP_INSTABILITY_ANALYSIS_H_

#include "InterpreterObserver.h"
#include "IValue.h"

using namespace llvm;

class FPInstabilityAnalysis : public InterpreterObserver {

  public:
    FPInstabilityAnalysis(std::string name) : InterpreterObserver(name) {}

    virtual void post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx);

    virtual void post_create_global_symbol_table();

  private:
    /**
     * Copy shadow value (long double value) from the source IValue to the
     * destination IValue.
     */
    static void copyShadow(IValue *src, IValue *dest);

    /**
     * Return long double shadow value for the given KVALUE.
     *
     * @note kv must has floating-point type.
     * @param kv the KVALUE to get shadow value from.
     * @return long double shadow value.
     */
    long double getShadowValue(SCOPE scope, int64_t value);

    long double getConcreteValue(SCOPE scope, int64_t value);

    int getLineNumber(SCOPE scope, int64_t value);

    int getEBits(long double v1, long double v2);

    int getExponent(double v);

    void fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx, BINOP op);

    /*
     * not used in PLDI12 Benz Paper
     virtual void fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx);

     virtual void fptoui(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);

     virtual void fptosi(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);

     virtual void fptrunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);

     virtual void fpext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);

      */


};

#endif /* FP_INSTABILITY_ANALYSIS_H_ */

