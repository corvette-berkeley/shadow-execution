/**
 * @file BlameTreeUtilities.cpp
 * @brief 
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

#include "BlameTreeUtilities.h"


double BlameTreeUtilities::clearBits(double v, int shift) {
  int64_t *ptr;
  double *dm;
  int64_t mask = 0xffffffffffffffff;
  mask = mask << shift; // adds "positions" zeros to the end

  ptr = (int64_t*)&v;
  *ptr = *ptr & mask;
  dm = (double*)ptr;

  return *dm;
}

BlameTree::HIGHPRECISION BlameTreeUtilities::eval(BlameTree::HIGHPRECISION value01,
    BlameTree::HIGHPRECISION value02, BINOP bop) {
  switch (bop) {
    case ADD:
    case FADD:
      return value01 + value02;
    case SUB:
    case FSUB:
      return value01 - value02; 
    case MUL:
    case FMUL:
      return value01 * value02;
    case FDIV:
    case UDIV:
    case SDIV:
      return value01 / value02;
    case UREM:
    case SREM:
    case FREM:
    default:
      safe_assert(false);
      DEBUG_STDERR("Unsupport binary operator " << bop);
      return 0;
  }
}

string BlameTreeUtilities::precisionToString(BlameTree::PRECISION precision) {
  switch (precision) {
    case BlameTree::BITS_23:
      return "23bits";
    case BlameTree::BITS_30:
      return "30bits";
    case BlameTree::BITS_37:
      return "37bits";
    case BlameTree::BITS_44:
      return "44bits";
    case BlameTree::BITS_52:
      return "52bits";
    default:
      return "undefined";
  }
}
