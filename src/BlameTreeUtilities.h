/**
 * @file BlameTreeUtilities.h
 * @brief BlameTreeUtilities Declarations.
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

#ifndef BLAME_TREE_UTILITIES_H
#define BLAME_TREE_UTILITIES_H

#include "Common.h"
#include "Constants.h"
#include <inttypes.h>
#include <map>
#include <set>
#include <string>
#include <vector>


using namespace std;

typedef double HIGHPRECISION;
typedef float LOWPRECISION;
typedef enum {BITS_23, BITS_30, BITS_37, BITS_44, BITS_52, PRECISION_NO} PRECISION;


class BlameTreeUtilities {
  public:
    static double clearBits(double v, int shift);

    /**
     * Evaluate the binary operation given the operator and two operands.
     *
     * @param value01 the first operand
     * @param value02 the second operand
     * @param bop the binary operator
     *
     * @return the result of the binary operation 
     */
    static HIGHPRECISION eval(HIGHPRECISION value01, HIGHPRECISION value02, BINOP bop);

    static int exactBits(PRECISION precision);

    static string precisionToString(PRECISION);
};

#endif
