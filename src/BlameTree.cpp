/**
 * @file BlameTree.cpp
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

#include "BlameTree.h"

/******* DEFINE ANALYSIS PARAMETERS *******/
int BlameTree::outputPC = 0;
HIGHPRECISION BlameTree::errorThreshold = 2e-26; 
HIGHPRECISION BlameTree::machineEpsilon = 2e-52;
map<int, vector<BlameTreeShadowObject<HIGHPRECISION> > > BlameTree::trace;
int BlameTree::dynamicCounter = 0;

/******* HELPER FUNCTIONS *******/

void BlameTree::copyShadow(IValue *src, IValue *dest) {
  //
  // copy shadow object from source to destination, only if they are
  // floating-point value
  //
  if (src->isFlpValue() && dest->isFlpValue()) {
    if (src->getShadow() != NULL) {
      BlameTreeShadowObject<HIGHPRECISION> *btmSOSrc, *btmSODest;

      btmSOSrc = (BlameTreeShadowObject<HIGHPRECISION>*) src->getShadow();
      btmSODest = new BlameTreeShadowObject<HIGHPRECISION>(*btmSOSrc);

      dest->setShadow(btmSODest);
    } else {
      dest->setShadow(NULL); 
    }
  }
}

void BlameTree::setShadow(SCOPE scope, int64_t inx, BlameTreeShadowObject<HIGHPRECISION>* shadowObject) {

  if (scope == CONSTANT) {
    return; // no need to associate this shadow object with any IValue
  } else {
    IValue *iv;
    if (scope == GLOBAL) {
      iv = globalSymbolTable[inx];
    }
    else {
      iv = executionStack.top()[inx];
    }
    iv->setShadow(shadowObject);
  }
  return;
}

BlameTreeShadowObject<HIGHPRECISION>* BlameTree::getShadowObject(SCOPE scope, int64_t inx) {

  if (scope == CONSTANT) {
    return NULL;
  } else {
    IValue *iv;
    if (scope == GLOBAL) {
      iv = globalSymbolTable[inx];
    }
    else {
      iv = executionStack.top()[inx];
    }
    return (BlameTreeShadowObject<HIGHPRECISION>*)iv->getShadow();
  }
}


HIGHPRECISION BlameTree::getShadowValue(SCOPE scope, int64_t inx, PRECISION precision) {
  HIGHPRECISION result;

  if (scope == CONSTANT) {
    double *ptr;
    ptr = (double*) &inx;
    result = *ptr;

  } else {
    IValue *iv;
    if (scope == GLOBAL) {
      iv = globalSymbolTable[inx];
    }
    else {
      iv = executionStack.top()[inx];
    }

    if (iv->getShadow() == NULL) {
      result = iv->getFlpValue();
    }
    else {
      result = ((BlameTreeShadowObject<HIGHPRECISION>*) iv->getShadow())->getValue(precision);
    }
  }

  return result;
}


LOWPRECISION BlameTree::getActualValue(SCOPE scope, int64_t value) {
  LOWPRECISION actualValue;

  if (scope == CONSTANT) {
    double *ptr;

    ptr = (double *) &value;
    actualValue = *ptr;
  } else {
    IValue *iv;

    iv = (scope == GLOBAL) ? globalSymbolTable[value] :
      executionStack.top()[value];
    actualValue = iv->getFlpValue();
  }

  return actualValue;
}

int BlameTree::getPC(SCOPE scope, int64_t value) {
  int pc;

  if (scope == CONSTANT) {
    pc = -1;
  } else {
    IValue *iv;

    iv = (scope == GLOBAL) ? globalSymbolTable[value] : executionStack.top()[value];
    if (iv->getShadow() != NULL) {
      pc = ((BlameTreeShadowObject<HIGHPRECISION>*) iv->getShadow())->getPC();
    } else {
      pc = iv->getLineNumber();
    }
  }

  return pc;
}

/******* ANALYSIS FUNCTIONS *******/

void BlameTree::pre_fpbinop(int inx) {
  if (executionStack.top()[inx]->getShadow() != NULL) {
    preBtmSO = *((BlameTreeShadowObject<HIGHPRECISION>*)
      executionStack.top()[inx]->getShadow());
  } else {
    preBtmSO = BlameTreeShadowObject<HIGHPRECISION>();
  }
}

void BlameTree::post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue,
    int64_t rValue, KIND type, int line UNUSED, int inx UNUSED, BINOP op) {

  BlameTreeShadowObject<HIGHPRECISION> *s1, *s2;
  HIGHPRECISION sv1, sv2, sresult;
  LOWPRECISION v1, v2;
  int pc1, pc2;

  //
  // assert: type is a floating-point type
  //
  safe_assert(type == FLP32_KIND || type == FLP64_KIND || type == FLP80X86_KIND
      || type == FLP128_KIND || type == FLP128PPC_KIND);

  //
  // Obtain actual value, shadow value and pc of two operands
  //
  v1 = getActualValue(lScope, lValue);
  v2 = getActualValue(rScope, rValue);
  
  s1 = getShadowObject(lScope, lValue);
  s2 = getShadowObject(rScope, rValue);

  pc1 = (lScope == CONSTANT) ? line : getPC(lScope, lValue); 
  pc2 = (rScope == CONSTANT) ? line : getPC(rScope, rValue);

  //
  // Perform binary operation for shadow values
  //
  HIGHPRECISION *values = new HIGHPRECISION[5];

  for(int i = 0; i < 5; i++) {
    if (s1) {
      sv1 = s1->getValue(i); 
    }
    else {
      sv1 = getShadowValue(lScope, lValue, (PRECISION)i);
    }
    if (s2) {
      sv2 = s2->getValue(i);
    }
    else {
      sv2 = getShadowValue(rScope, rValue, (PRECISION)i);
    }

    switch (op) {
    case FADD:
      sresult = sv1 + sv2;
      break;
    case FSUB:
      sresult = sv1 - sv2;
      break;
    case FMUL:
      sresult = sv1 * sv2;
      break;
    case FDIV:
      sresult = sv1 / sv2;
      break;
    default:
      DEBUG_STDERR("Unsupported floating-point binary operator: " << BINOP_ToString(op)); 
      safe_assert(false);
    }

    switch(i) {
    case BITS_23:
      values[BITS_23] = BlameTreeUtilities::clearBits(sresult, 52 - 23); // TODO: test independently rounding vs truncation
      break;
    case BITS_30:
      values[BITS_30] = BlameTreeUtilities::clearBits(sresult, 52 - 30);
      break;
    case BITS_37:
      values[BITS_37] = BlameTreeUtilities::clearBits(sresult, 52 - 37);
      break;
    case BITS_44:
      values[BITS_44] = BlameTreeUtilities::clearBits(sresult, 52 - 44);
      break;
    case BITS_52:
      values[BITS_52] = sresult;
      break;
    default:
      // nothing
      break;
    }
  }

  // creating, recording, and printing shadow object for target
  BlameTreeShadowObject<HIGHPRECISION> resultShadow(pc1, dynamicCounter,
      BlameTreeShadowObject<HIGHPRECISION>::BIN_INTR, op, values);
  trace[resultShadow.getDPC()].push_back(resultShadow);

  // shadow objects for operands
  if (!s1) {
    // constructing and setting shadow object
    s1 = new BlameTreeShadowObject<HIGHPRECISION>();
    s1->setValue(BITS_23, v1);
    s1->setValue(BITS_30, BlameTreeUtilities::clearBits((HIGHPRECISION)v1, 52-30));
    s1->setValue(BITS_37, BlameTreeUtilities::clearBits((HIGHPRECISION)v1, 52-37));
    s1->setValue(BITS_44, BlameTreeUtilities::clearBits((HIGHPRECISION)v1, 52-44));
    s1->setValue(BITS_52, (HIGHPRECISION)v1);
    s1->setPC(pc1);
    s1->setDPC(dynamicCounter);
    setShadow(lScope, lValue, s1);
    //s1->print();
  }
  if (!s2) {
    // constructing and setting shadow object
    s2 = new BlameTreeShadowObject<HIGHPRECISION>();
    s2->setValue(BITS_23, v2);
    s2->setValue(BITS_30, BlameTreeUtilities::clearBits((HIGHPRECISION)v2, 52-30));
    s2->setValue(BITS_37, BlameTreeUtilities::clearBits((HIGHPRECISION)v2, 52-37));
    s2->setValue(BITS_44, BlameTreeUtilities::clearBits((HIGHPRECISION)v2, 52-44));
    s2->setValue(BITS_52, (HIGHPRECISION)v2);
    s2->setPC(pc2);
    s2->setDPC(dynamicCounter);
    setShadow(rScope, rValue, s2);
    //s2->print();
  }
  
  // making copies of shadow objects
  BlameTreeShadowObject<HIGHPRECISION> s1Copy(*s1);
  BlameTreeShadowObject<HIGHPRECISION> s2Copy(*s2);

  // adding to the trace
  trace[dynamicCounter].push_back(s1Copy);
  trace[dynamicCounter].push_back(s2Copy);

  dynamicCounter++;
  return;
}

void BlameTree::pre_fadd(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_fpbinop(inx);
}

void BlameTree::pre_fsub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_fpbinop(inx);
}

void BlameTree::pre_fmul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_fpbinop(inx);
}

void BlameTree::pre_fdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_fpbinop(inx);
}

void BlameTree::post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FADD);
}

void BlameTree::post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FSUB);
}

void BlameTree::post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FMUL);
}

void BlameTree::post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FDIV);
}

void BlameTree::post_analysis() {
  cout << "Printing trace after analysis: " << dynamicCounter << endl;
  cout << "Trace length: " << dynamicCounter << endl;
  /*
  for(int i = 0; i < dynamicCounter; i++) {
    cout << "Counter: " << i << endl;
    for(unsigned j = 0; j < trace[i].size(); j++) {
      cout << "\t";
      trace[i][j].print();
    }
  }
  */
  return;
}
