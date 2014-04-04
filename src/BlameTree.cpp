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
BlameTree::HIGHPRECISION BlameTree::errorThreshold = 2e-26; 
BlameTree::HIGHPRECISION BlameTree::machineEpsilon = 2e-52;
map<int, vector<BlameTreeShadowObject<BlameTree::HIGHPRECISION> > > BlameTree::trace;
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


BlameTree::HIGHPRECISION BlameTree::getShadowValue(SCOPE scope, int64_t inx, PRECISION precision) {
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


BlameTree::LOWPRECISION BlameTree::getActualValue(SCOPE scope, int64_t value) {
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

BlameTree::HIGHPRECISION BlameTree::computeRelativeError(HIGHPRECISION
                 highValue, LOWPRECISION lowValue) {
  HIGHPRECISION d; 
  
  d = highValue != 0 ? highValue : machineEpsilon;

  return fabs((HIGHPRECISION)((highValue - lowValue)/d));
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

  HIGHPRECISION sv1, sv2, sresult;
  //LOWPRECISION v1, v2;
  int pc1;

  //
  // assert: type is a floating-point type
  //
  safe_assert(type == FLP32_KIND || type == FLP64_KIND || type == FLP80X86_KIND
      || type == FLP128_KIND || type == FLP128PPC_KIND);

  //
  // Obtain actual value, shadow value and pc of two operands
  //
  //v1 = getActualValue(lScope, lValue);
  //v2 = getActualValue(rScope, rValue);
  sv1 = getShadowValue(lScope, lValue, BITS_52);
  sv2 = getShadowValue(rScope, rValue, BITS_52);
  pc1 = (lScope == CONSTANT) ? line : getPC(lScope, lValue); 
  //pc2 = (rScope == CONSTANT) ? line : getPC(rScope, rValue);

  //
  // Perform binary operation for shadow values
  //
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


  HIGHPRECISION *values = new HIGHPRECISION[5];
  values[BITS_52] = sresult;
  BlameTreeShadowObject<HIGHPRECISION> resultShadow(pc1, dynamicCounter++, BIN_INTR, op, values);
  cout << "[TRACE] address: " << &resultShadow << ", dpc: " << resultShadow.getDPC() << ", value: " << resultShadow.getValue(BITS_52) << endl;
  trace[resultShadow.getDPC()].push_back(resultShadow);

  //
  // Compute other analysis information such as relative error, sources of
  // relative error
  //
}


void BlameTree::pre_load(IID iid UNUSED, KIND type UNUSED, SCOPE opScope UNUSED, int opInx UNUSED, uint64_t opAddr UNUSED, bool 
				   loadGlobal UNUSED, int loadInx UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {}
 
void BlameTree::post_load(IID iid UNUSED, KIND type UNUSED, SCOPE opScope UNUSED, int opInx UNUSED, uint64_t opAddr UNUSED, bool 
				   loadGlobal UNUSED, int loadInx UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {
  dynamicCounter++;
}

void BlameTree::pre_store(int destInx UNUSED, SCOPE destScope UNUSED, KIND srcKind UNUSED, SCOPE srcScope UNUSED, int srcInx UNUSED, int64_t srcValue UNUSED, 
				    int file UNUSED, int line UNUSED, int inx UNUSED) {}

void BlameTree::post_store(int destInx UNUSED, SCOPE destScope UNUSED, KIND srcKind UNUSED, SCOPE srcScope UNUSED, int srcInx UNUSED, int64_t srcValue UNUSED, 
				     int file UNUSED, int line UNUSED, int inx UNUSED) {
  dynamicCounter++;
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
  for(int i = 0; i < dynamicCounter; i++) {
    cout << "Counter: " << i << endl;
    for(unsigned j = 0; j < trace[i].size(); j++) {
      cout << "\t";
      trace[i][j].print();
    }
  }
  return;
}
