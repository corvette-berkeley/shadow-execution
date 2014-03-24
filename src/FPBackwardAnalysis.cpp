/**
 * @file FPBackwardAnalysis.cpp
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

#include "FPBackwardAnalysis.h"
#include "FPBackwardShadowObject.h"
#include <math.h>
#include <queue>
#include <stack>
#include <llvm/IR/InstrTypes.h>

/******** INITIALIZE ANALYSIS CONFIGURATIONS **********/
int FPBackwardAnalysis::source = 10;
double FPBackwardAnalysis::epsilon = -10;

/******** HELPER FUNCTIONS **********/

void FPBackwardAnalysis::copyShadow(IValue *src, IValue *dest) {
  //
  // copy shadow value from src to dest
  //
  if (src->isFlpValue() && dest->isFlpValue()) {
    if (src->getShadow() != NULL) {
      FPBackwardShadowObject *fpISOSrc, *fpISODest; 

      fpISOSrc = (FPBackwardShadowObject*) src->getShadow();
      fpISODest = new FPBackwardShadowObject(0,0);
      fpISOSrc->copyTo(fpISODest);

      dest->setShadow(fpISODest);
    } else {
      dest->setShadow(NULL);
    }
  }
}

long double FPBackwardAnalysis::getShadowValue(SCOPE scope, int64_t value) {
  long double result;

  if (scope == CONSTANT) {
    double *ptr;

    ptr = (double *) &value;
    result = *ptr;
  } else {
    IValue *iv;

    iv = (scope == GLOBAL) ? globalSymbolTable[value] : executionStack.top()[value];
    result = iv->getShadow() == NULL ? (long double) iv->getFlpValue() : 
        ((FPBackwardShadowObject*) iv->getShadow())->getValue();
  }

  return result;
}

long double FPBackwardAnalysis::getConcreteValue(SCOPE scope, int64_t value) {
  long double result;

  if (scope == CONSTANT) {
    double *ptr;

    ptr = (double *) &value;
    result = *ptr;
  } else {
    IValue *iv;

    iv = (scope == GLOBAL) ? globalSymbolTable[value] : executionStack.top()[value];
    result = iv->getFlpValue();
  }

  return result;
}

int FPBackwardAnalysis::getLineNumber(SCOPE scope, int64_t value) {
  int line;

  if (scope == CONSTANT) {
    line = -1;
  } else {
    IValue *iv;

    iv = (scope == GLOBAL) ? globalSymbolTable[value] : executionStack.top()[value];
    if (iv->getShadow() != NULL) {
      line = ((FPBackwardShadowObject*) iv->getShadow())->getLine();
    } else {
      line = iv->getLineNumber();
    }
  }

  return line;
}



/******** ANALYSIS FUNCTIONS **********/

void FPBackwardAnalysis::pre_analysis(int inx) {
  if (executionStack.top()[inx]->getShadow() != NULL) {
    preFpISO = (FPBackwardShadowObject*) executionStack.top()[inx]->getShadow();
  } else {
    preFpISO = new FPBackwardShadowObject(0, 0);
  }
}

void FPBackwardAnalysis::post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx, BINOP op) {

  //long double v1, v2, result;
  PRECISION sv1, sv2, sresult;
  FPBackwardShadowObject *fpISO;
  int l1, l2;

  //
  // assert: type is a floating-point type.
  //
  safe_assert(type == FLP32_KIND || type == FLP64_KIND || type == FLP80X86_KIND
      || type == FLP128_KIND || type == FLP128PPC_KIND);

  //
  // Obtain shadow value (long double value) from the two operands.
  //
  //v1 = getConcreteValue(lScope, lValue);
  //v2 = getConcreteValue(rScope, rValue);
  sv1 = getShadowValue(lScope, lValue);
  sv2 = getShadowValue(rScope, rValue);
  l1 = (lScope == CONSTANT) ? line : getLineNumber(lScope, lValue);
  l2 = (rScope == CONSTANT) ? line : getLineNumber(rScope, rValue);

  //
  // Compute the operation for two shadow values.
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

  // set result in shadow object + absolute error
  cout << "********lines: " << l1 << ":" << l2 << " sresult: " << sresult << endl;

  //
  // Construct shadow value for the result shadow object. 
  //
  if (executionStack.top()[inx]->getShadow() == NULL) {
    fpISO = new FPBackwardShadowObject(sresult, line);
  } else {
    fpISO = (FPBackwardShadowObject*) executionStack.top()[inx]->getShadow();
  }
  cout << "\tConcrete result: " << executionStack.top()[inx]->getFlpValue() << endl;
  cout << "\tAbsolute error: " << fabs(sresult - executionStack.top()[inx]->getFlpValue()) << endl;

  cout << "====";
  fpISO->print();

  delete(preFpISO);
  preFpISO = NULL;
  
  executionStack.top()[inx]->setShadow(fpISO);
  return;
}

void FPBackwardAnalysis::pre_fadd(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPBackwardAnalysis::pre_fsub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPBackwardAnalysis::pre_fmul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPBackwardAnalysis::pre_fdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPBackwardAnalysis::post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FADD);
}

void FPBackwardAnalysis::post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FSUB);
}

void FPBackwardAnalysis::post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FMUL);
}

void FPBackwardAnalysis::post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, line, inx, FDIV);
}

/*
void FPBackwardAnalysis::post_create_global_symbol_table() {
  //
  // instantiate copyShadow
  //
  IValue::setCopyShadow(&copyShadow);
}
*/
void FPBackwardAnalysis::pre_fptoui(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}

void FPBackwardAnalysis::pre_fcmp(SCOPE lScope, SCOPE rScope UNUSED, int64_t lValue, int64_t rValue UNUSED, KIND type UNUSED, PRED pred UNUSED, int line UNUSED, int inx UNUSED) {
  if (lScope != CONSTANT) {
    pre_analysis(lValue);
  }
}
void FPBackwardAnalysis::pre_fptosi(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}


void FPBackwardAnalysis::pre_fptrunc(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}


void FPBackwardAnalysis::pre_fpext(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}

