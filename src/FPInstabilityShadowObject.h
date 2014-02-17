/**
 * @file FPInstabilityShadowObject.h
 * @brief FPInstabilityShadowObject Declarations.
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

#ifndef FP_INSTABILITY_SHADOW_OBJECT_H
#define FP_INSTABILITY_SHADOW_OBJECT_H

class FPInstabilityShadowObject {
  private:
    long double val;    // Value in higher precision
    int td;             // Depth of the computation tree
    int pc;             // Address of the instruction that wrote to the variable lastly
    int mcb;            // Maximum cancellation badness
    int mcbSrc;         // Instruction that caused maximum cancellation badness
    long double sre;    // Sum or relative errors
    long double mre;    // Maximum relative error
    int mreSrc[2];      // Pair of instruction addresses that point to the instructions 
                        // that computed the operands that caused the maximum relative
                        // error
    int scb;            // Sum of cancellation badnesses

  public:

    FPInstabilityShadowObject(long double v, int line): val(v), td(1), pc(line), mcb(0), mcbSrc(-1), sre(0), mre(0), scb(0) {
      mreSrc[0] = -1;
      mreSrc[1] = -1;
    }

    long double getValue();

    int getComputationDepth();

    int getPC();

    int getMaxCBad();

    int getMaxCBadSource();

    long double getSumRelErr();

    long double getMaxRelErr();

    int getMaxRelErrSource(int i);

    int getSumCBad();

    void setValue(long double val);

    void setComputationDepth(int td);

    void setPC(int pc);

    void setMaxCBad(int mcb);

    void setMaxCBadSource(int mcbSrc);

    void setSumRelErr(long double sre);

    void setMaxRelErr(long double mre);

    void setMaxRelErrSource(int i, int pc);

    void setSumCBad(int scb);

    void copyTo(FPInstabilityShadowObject *fpISO);
};

#endif /* FP_INSTABILITY_SHADOW_OBJECT_H_ */
