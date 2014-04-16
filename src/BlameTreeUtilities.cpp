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

bool BlameTreeUtilities::equalWithPrecision(double v1, double v2, PRECISION prec) {
  int64_t *ptr1, *ptr2;

  if (prec == BITS_DOUBLE) {
    return v1 == v2;
  }

  ptr1 = (int64_t*)&v1;
  ptr2 = (int64_t*)&v2;

  *ptr1 = *ptr1 << 12 >> 12 >> (52 - exactBits(prec) - 1);
  *ptr2 = *ptr2 << 12 >> 12 >> (52 - exactBits(prec) - 1);

  return abs(*ptr1 - *ptr2) <= 1;
}

double BlameTreeUtilities::clearBits(double v, int shift) {
  int64_t *ptr;
  double *dm;
  int64_t mask = 0xffffffffffffffff;
  mask = mask << shift;

  ptr = (int64_t*)&v;
  *ptr = *ptr & mask;
  dm = (double*) ptr;

  return *dm;

  /*
  if (shift != 0) {
    double v2;
    int64_t *ptr;
    int64_t *ptr2;
    double *dm;
    double *dm2;
    int64_t mask = 0xffffffffffffffff;
    int64_t mask2 = mask;

    mask = mask << shift;
    mask2 = mask2 << (shift-1);
    v2 = v;

    ptr = (int64_t*)&v;
    ptr2 = (int64_t*)&v2;
    *ptr = *ptr & mask;
    *ptr2 = *ptr2 & mask2;
    dm = (double*)ptr;
    dm2 = (double*)ptr2;

    if (*dm == *dm2) {
      return *dm;
    } else {
      // need to round up dm
      int64_t round = *ptr;
      round = round >> shift;
      round = round + 1; 
      round = round << shift;
      // over-flow
      safe_assert(round != 0);
      *ptr = round;
      dm = (double*)ptr;

      return *dm;
    }
  } 

  return v;
  */
}

LOWPRECISION BlameTreeUtilities::feval(LOWPRECISION value01, LOWPRECISION value02, BINOP bop) {
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

HIGHPRECISION BlameTreeUtilities::eval(HIGHPRECISION value01, HIGHPRECISION value02, BINOP bop) {
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
      DEBUG_STDERR("Unsupport binary operator " << bop);
      safe_assert(false);
      return 0;
  }
}

HIGHPRECISION BlameTreeUtilities::evalFunc(HIGHPRECISION value, string func) {
  if (func.compare("sin") == 0) {
    return sin(value);
  } else if (func.compare("acos") == 0) {
    return acos(value);
  } else if (func.compare("sqrt") == 0) {
    return sqrt(value);
  } else {
    DEBUG_STDERR("Unsupport function " << func);
    safe_assert(false);
    return 0;
  }
}

int BlameTreeUtilities::exactBits(PRECISION precision) {
   switch (precision) {
     /*
     case BITS_8:
       return 8;
     case BITS_9:
       return 9;
     case BITS_10:
       return 10;
     case BITS_11:
       return 11;
       */
     case BITS_12:
       return 12;
     case BITS_13:
       return 13;
     case BITS_14:
       return 14;
     case BITS_15:
       return 15;
     case BITS_16:
       return 16;
     case BITS_17:
       return 17;
     case BITS_18:
       return 18;
     case BITS_19:
       return 19;
     case BITS_20:
       return 20;
     case BITS_21:
       return 21;
     case BITS_22:
       return 22;
     case BITS_23:
       return 23;
     case BITS_24:
       return 24;
     case BITS_25:
       return 25;
     case BITS_26:
       return 26;
     case BITS_27:
       return 27;
     case BITS_28:
       return 28;
     case BITS_29:
       return 29;
     case BITS_30:
       return 30;
     case BITS_31:
       return 31;
     case BITS_32:
       return 32;
     case BITS_33:
       return 33;
     case BITS_DOUBLE:
       return 52;
     default:
       safe_assert(false);
       return 0;
   }
}

PRECISION BlameTreeUtilities::exactBitToPrecision(int bit) {
  switch (bit) {
    /*
    case 8:
      return BITS_8;
    case 9:
      return BITS_9;
    case 10:
      return BITS_10;
    case 11:
      return BITS_11;
      */
    case 12:
      return BITS_12;
    case 13:
      return BITS_13;
    case 14:
      return BITS_14;
    case 15:
      return BITS_15;
    case 16:
      return BITS_16;
    case 17:
      return BITS_17;
    case 18:
      return BITS_18;
    case 19:
      return BITS_19;
    case 20:
      return BITS_20;
    case 21:
      return BITS_21;
    case 22:
      return BITS_22;
    case 23:
      return BITS_23;
    case 24:
      return BITS_24;
    case 25:
      return BITS_25;
    case 26:
      return BITS_26;
    case 27:
      return BITS_27;
    case 28:
      return BITS_28;
    case 29:
      return BITS_29;
    case 30:
      return BITS_30;
    case 31:
      return BITS_31;
    case 32:
      return BITS_32;
    case 33:
      return BITS_33;
    case 52:
      return BITS_DOUBLE;
    default:
      safe_assert(false);
      return BITS_FLOAT;
  }
}

string BlameTreeUtilities::precisionToString(PRECISION precision) {
  switch (precision) {
    case BITS_FLOAT:
      return "float";
      /*
    case BITS_8:
      return "8 bits";
    case BITS_9:
      return "9 bits";
    case BITS_10:
      return "10 bits";
    case BITS_11:
      return "11 bits";
      */
    case BITS_12:
      return "12 bits";
    case BITS_13:
      return "13 bits";
    case BITS_14:
      return "14 bits";
    case BITS_15:
      return "15 bits";
    case BITS_16:
      return "16 bits";
    case BITS_17:
      return "17 bits";
    case BITS_18:
      return "18 bits";
    case BITS_19:
      return "19 bits";
    case BITS_20:
      return "20 bits";
    case BITS_21:
      return "21 bits";
    case BITS_22:
      return "22 bits";
    case BITS_23:
      return "23 bits";
    case BITS_24:
      return "24 bits";
    case BITS_25:
      return "25 bits";
    case BITS_26:
      return "26 bits";
    case BITS_27:
      return "27 bits";
    case BITS_28:
      return "28 bits";
    case BITS_29:
      return "29 bits";
    case BITS_30:
      return "30 bits";
    case BITS_31:
      return "31 bits";
    case BITS_32:
      return "32 bits";
    case BITS_33:
      return "33bits";
    case BITS_DOUBLE:
      return "double";
    default:
      safe_assert(false);
      return "undefined";
  }
}
