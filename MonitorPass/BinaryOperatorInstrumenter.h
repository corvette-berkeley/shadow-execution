/**
 * @file BinaryOperatorInstrumenter.h
 * @brief BinaryOperatorInstrumenter Declarations.
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

// Author: Cindy Rubio-Gonzalez and Cuong Nguyen

#ifndef BINARY_OPERATORS_INSTRUMENTER_H_
#define BINARY_OPERATORS_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class BinaryOperatorInstrumenter : public Instrumenter {
  public:
    /**
     * Constructor of BinaryOperatorInstrumenter class.
     */
    BinaryOperatorInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {}

    virtual bool CheckAndInstrument(Instruction* inst);
  private:
    /**
     * Get BINOP value for the given instruction.
     *
     * @note BINOP is defined in Constants.h.
     *
     * @param inst the LLVM instruction.
     *
     * @return the BINOP value associated with the given instruction.
     */
    BINOP getBinOp(BinaryOperator* inst); 

    BITWISE getBitWise(BinaryOperator* inst);

};

#endif /* BINARY_OPERATORS_INSTRUMENTER_H_ */
