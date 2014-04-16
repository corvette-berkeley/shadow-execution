/**
 * @file FPInstabilityAnalysis.cpp
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

// Author: Cuong Nguyen

#include "FPInstabilityAnalysis.h"
#include "FPInstabilityShadowObject.h"
#include <math.h>
#include <queue>
#include <stack>
#include <llvm/IR/InstrTypes.h>

/******** INITIALIZE ANALYSIS CONFIGURATIONS **********/
int FPInstabilityAnalysis::source = 51;
double FPInstabilityAnalysis::epsilon = -4;

/******** HELPER FUNCTIONS **********/

void FPInstabilityAnalysis::copyShadow(IValue *src, IValue *dest) {
  //
  // copy shadow value from src to dest
  //
  if (src->isFlpValue() && dest->isFlpValue()) {
    if (src->getShadow() != NULL) {
      FPInstabilityShadowObject<HIGHPRECISION> *fpISOSrc, *fpISODest; 

      fpISOSrc = (FPInstabilityShadowObject<HIGHPRECISION> *) src->getShadow();
      fpISODest = new FPInstabilityShadowObject<HIGHPRECISION>(0,0,0, BINOP_INVALID);
      fpISOSrc->copyTo(fpISODest);

      dest->setShadow(fpISODest);
    } else {
      dest->setShadow(NULL);
    }
  }
}

FPInstabilityAnalysis::HIGHPRECISION FPInstabilityAnalysis::computeRelativeError(HIGHPRECISION highValue, float lowValue) {
  HIGHPRECISION d = highValue != 0 ? highValue : 10e-10;
  return abs((HIGHPRECISION)((highValue - lowValue)/d));
}

long double FPInstabilityAnalysis::getShadowValue(SCOPE scope, int64_t value) {
  long double result;

  if (scope == CONSTANT) {
    double *ptr;

    ptr = (double *) &value;
    result = *ptr;
  } else {
    IValue *iv;

    iv = (scope == GLOBAL) ? globalSymbolTable[value] : executionStack.top()[value];
    result = iv->getShadow() == NULL ? (long double) iv->getFlpValue() : 
        ((FPInstabilityShadowObject<HIGHPRECISION>*) iv->getShadow())->getValue();
  }

  return result;
}

long double FPInstabilityAnalysis::getConcreteValue(SCOPE scope, int64_t value) {
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

int FPInstabilityAnalysis::getLineNumber(SCOPE scope, int64_t value) {
  int line;

  if (scope == CONSTANT) {
    line = -1;
  } else {
    IValue *iv;

    iv = (scope == GLOBAL) ? globalSymbolTable[value] : executionStack.top()[value];
    if (iv->getShadow() != NULL) {
      line = ((FPInstabilityShadowObject<HIGHPRECISION>*) iv->getShadow())->getPC();
    } else {
      line = iv->getLineNumber();
    }
  }

  return line;
}

int64_t FPInstabilityAnalysis::getMantisa(double v) {
  int64_t m;
  int64_t *ptr;

  ptr = (int64_t*)&v;
  m = *ptr & 0x000fffffffffffff;

  return m;
}

int FPInstabilityAnalysis::getExactBits(double v1, double v2) {
  int64_t m1, m2;

  m1 = getMantisa(v1);
  m2 = getMantisa(v2);

  if (m1 == m2) {
    return 53; // significand bit of double
  } else {
    int ie = 0;

    while (m1 != m2 && ie < 52) {
      m1 >>= 1;
      m2 >>= 1;
      ie += 1;
    }

    return 53 - ie;
  }
}

int FPInstabilityAnalysis::getEBits(double v1, double v2) {
  int p = 53; // significand bit of double

  if (v1 == v2) {
    return p;
  } else {
    int e1, e2;

    e1 = getExponent(v1);
    e2 = getExponent(v2);
    if (e1 != e2) {
      return 0;
    } else {
      int e3;

      e3 = getExponent(v1-v2);
      return min(p, abs(e1-e3));
    }
  }
}

int FPInstabilityAnalysis::getExponent(double v) {
  int e;
  int *ptr; 

  ptr = (int*)&v;
  ptr = ptr + 1;
  e = *ptr & 0x7ff00000;
  e >>= 20;
  e = e - 1023;

  return e;
}

/******** ANALYSIS FUNCTIONS **********/

void FPInstabilityAnalysis::pre_analysis(int inx) {
  if (executionStack.top()[inx]->getShadow() != NULL) {
    preFpISO = (FPInstabilityShadowObject<HIGHPRECISION>*) executionStack.top()[inx]->getShadow();
  } else {
    preFpISO = new FPInstabilityShadowObject<HIGHPRECISION>(0,0, 0, BINOP_INVALID);
  }
}

void FPInstabilityAnalysis::pre_sync_call(int inx UNUSED, int line UNUSED) {
  if (executionStack.top()[inx]->isFlpValue()) {
    preFpISO = new FPInstabilityShadowObject<HIGHPRECISION>(0,0, line, BINOP_INVALID);
    preFpISO->setMaxRelErr(pow(10, epsilon));
    if (executionStack.top()[inx]->getShadow() != NULL) {
      preFpISO->setSumRelErr(pow(10, epsilon) +
          ((FPInstabilityShadowObject<HIGHPRECISION>*)
           executionStack.top()[inx]->getShadow())->getSumRelErr());
    }

    stack<IValue*> params (callArgs);
    int paramCnt = 0;

    while (!params.empty() && paramCnt < 2) {
      IValue *iValue = params.top();
      params.pop();

      if (iValue->isFlpValue() && iValue->getShadow() != NULL) {
        preFpISO->setMaxRelErrSource(paramCnt, ((FPInstabilityShadowObject<HIGHPRECISION>*) iValue->getShadow())->getPC());
        paramCnt++;
      }
    }
  }
}

void FPInstabilityAnalysis::post_sync_call(int inx UNUSED, int line UNUSED) {
  if (executionStack.top()[inx]->isFlpValue()) {
    FPInstabilityShadowObject<HIGHPRECISION> *shadow = new FPInstabilityShadowObject<HIGHPRECISION>(*preFpISO);
    shadow->setAbnormalType(FPInstabilityShadowObject<HIGHPRECISION>::SYNC);
    shadow->setValue((executionStack.top()[inx]->getFlpValue())/(1-pow(10,epsilon)));
    //    shadow->setValue(executionStack.top()[inx]->getFlpValue());
    delete(preFpISO);
    preFpISO = NULL;
    executionStack.top()[inx]->setShadow(shadow);

    //
    // Assume that syncing always incurs high relative error
    //
    analysisTable[line] = *shadow;
  }
}

void FPInstabilityAnalysis::post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int inx, BINOP op) {

  HIGHPRECISION v1, v2, sv1, sv2, sresult, result;
  FPInstabilityShadowObject<HIGHPRECISION> *fpISO;
  HIGHPRECISION rerr;
  int l1, l2, e1, e2, e3, cbits, cbad;

  //
  // assert: type is a floating-point type.
  //
  safe_assert(type == FLP32_KIND || type == FLP64_KIND || type == FLP80X86_KIND
      || type == FLP128_KIND || type == FLP128PPC_KIND);

  //
  // Obtain shadow value (long double value) from the two operands.
  //
  v1 = getConcreteValue(lScope, lValue);
  v2 = getConcreteValue(rScope, rValue);
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

  //
  // Construct shadow value for the result shadow object. 
  //
  result = executionStack.top()[inx]->getFlpValue();
  if (executionStack.top()[inx]->getShadow() == NULL) {
    fpISO = new FPInstabilityShadowObject<HIGHPRECISION>(sresult, result, line, op);
  } else {
    fpISO = (FPInstabilityShadowObject<HIGHPRECISION>*) executionStack.top()[inx]->getShadow();
  }

  //
  // Compute cancellation badness and relative errors.
  // See: "A dyanmic program analysis to find floating-point accuracy problems"
  // by F. Benz, A. Hildebrandt and S. Hack, PLDO 2012, p.6
  //
  rerr = computeRelativeError(sresult, result);

  fpISO->setSumRelErr(preFpISO->getSumRelErr() + rerr);

  if (rerr > preFpISO->getMaxRelErr()) {
    fpISO->setMaxRelErr(rerr);
    fpISO->setMaxRelLowValue(0, v1); 
    fpISO->setMaxRelLowValue(1, v2);
    fpISO->setMaxRelHighValue(0, sv1);
    fpISO->setMaxRelHighValue(1, sv2);
    fpISO->setMaxRelErrSource(0, l1);
    fpISO->setMaxRelErrSource(1, l2);
    fpISO->setMaxRelErrValue(sresult);
  }

  e1 = getExponent(v1);
  e2 = getExponent(v2);
  e3 = getExponent(result);
  cbits = max(e1, e2) - e3;
  cbad = max(0, 1 + cbits - min(getEBits(sv1, v1), getEBits(sv2, v2)));
  fpISO->setSumCBad(preFpISO->getSumCBad() + cbad);
  if (cbad > preFpISO->getMaxCBad()) {
    fpISO->setMaxCBad(cbad);
    fpISO->setMaxCBadSource(line);
  } else {
    fpISO->setMaxCBad(preFpISO->getMaxCBad());
    fpISO->setMaxCBadSource(preFpISO->getMaxCBadSource());
  }

  fpISO->setValue(sresult);
  fpISO->setPC(line);

  delete(preFpISO);
  preFpISO = NULL;

  //
  // record analysis information
  //
  // if (fpISO->getMaxRelErr() > 0.7 && cbad > 5) {
  // if (fpISO->getMaxRelErr() > pow(10, epsilon)) {
  if (true) {
    fpISO->setAbnormalType(FPInstabilityShadowObject<HIGHPRECISION>::CANCELLATION);
    analysisTable[line] = *fpISO;
    DEBUG_STDERR("[WARN] Catastrophic cancellation at line: " << line << ", cbad: " << cbad);
    DEBUG_STDERR("\t sum relative error: " << fpISO->getSumRelErr());
    DEBUG_STDERR("\t max relative error: " << fpISO->getMaxRelErr());
    DEBUG_STDERR("\t max relative error source: " << fpISO->getMaxRelErrSource(0) << ":" << fpISO->getMaxRelErrSource(1));
    DEBUG_STDERR("\t max cbad: " << fpISO->getMaxCBad());
    DEBUG_STDERR("\t max cbad source: " << fpISO->getMaxCBadSource());
    DEBUG_STDERR("\t binop:" << BINOP_ToString(op));
    DEBUG_STDERR("\t v1:" << v1 << ", sv1:" << sv1);
    DEBUG_STDERR("\t v2:" << v2 << ", sv2:" << sv2);
    DEBUG_STDERR("\t result:" << result << ", sresult:" << sresult);

    /*
       cerr << "[WARN] Catastrophic cancellation at line: " << line << ", cbad: " << cbad << endl;
       cerr << "\t sum relative error: " << fpISO->getSumRelErr() << endl;
       cerr << "\t max relative error: " << fpISO->getMaxRelErr() << endl;
       cerr << "\t max relative error source: " << fpISO->getMaxRelErrSource(0) << ":" << fpISO->getMaxRelErrSource(1) << endl;
       cerr << "\t max cbad: " << fpISO->getMaxCBad() << endl;
       cerr << "\t max cbad source: " << fpISO->getMaxCBadSource() << endl;
       cerr << "\t binop:" << BINOP_ToString(op) << endl;
       cerr << "\t v1:" << v1 << ", sv1:" << sv1 << endl;
       cerr << "\t v2:" << v2 << ", sv2:" << sv2 << endl;
       cerr << "\t result:" << result << ", sresult:" << sresult << endl;
       */
  }

  executionStack.top()[inx]->setShadow(fpISO);

}

void FPInstabilityAnalysis::pre_fadd(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPInstabilityAnalysis::pre_fsub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPInstabilityAnalysis::pre_fmul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPInstabilityAnalysis::pre_fdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int inx) {
  pre_analysis(inx);
}

void FPInstabilityAnalysis::post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, file, line, inx, FADD);
}

void FPInstabilityAnalysis::post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, file, line, inx, FSUB);
}

void FPInstabilityAnalysis::post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, file, line, inx, FMUL);
}

void FPInstabilityAnalysis::post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line, int inx) {
  post_fbinop(lScope, rScope, lValue, rValue, type, file, line, inx, FDIV);
}

void FPInstabilityAnalysis::post_create_global_symbol_table() {
  //
  // instantiate copyShadow
  //
  IValue::setCopyShadow(&copyShadow);
}

void FPInstabilityAnalysis::pre_fptoui(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}

void FPInstabilityAnalysis::post_fptoui(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind, int size UNUSED, int inx) {

  double sv, cv; 
  int64_t sresult;


  cv = getConcreteValue(opScope, op);
  sv = getShadowValue(opScope, op);

  switch (kind) {
    case INT1_KIND:
      sresult = (bool) sv;
      break;
    case INT8_KIND:
      sresult = (uint8_t) sv;
      break;
    case INT16_KIND:
      sresult = (uint16_t) sv;
      break;
    case INT24_KIND:
    case INT32_KIND:
      sresult = (uint32_t) sv;
      break;
    case INT64_KIND:
      sresult = (uint64_t) sv;
      break;
    default:
      sresult = 0;
      DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(kind));
      safe_assert(false);
  }

  if (sresult != executionStack.top()[inx]->getIntValue()) {
    DEBUG_STDERR("[FPInstabilityAnalysis] Concrete difference at FPTOUI.");
    DEBUG_STDERR("[FPInstabilityAnalysis] Concrete difference at FPTOUI.");
    DEBUG_STDERR("\tshadow operand = " << sv << ", concrete operand = " << cv);
    DEBUG_STDERR("\tshadow result = " << sresult << ", result = " << executionStack.top()[inx]->getIntValue());
    if (preFpISO != NULL) {
      FPInstabilityShadowObject<HIGHPRECISION> fpISO(*preFpISO);
      fpISO.setAbnormalType(FPInstabilityShadowObject<HIGHPRECISION>::CONCRETEDIFFERENCE);
      // TODO: add fpISO to analysis table
      DEBUG_STDERR("\t sum relative error: " << preFpISO->getSumRelErr());
      DEBUG_STDERR("\t max relative error: " << preFpISO->getMaxRelErr());
      DEBUG_STDERR("\t max relative error source: " << preFpISO->getMaxRelErrSource(0) << ":" << preFpISO->getMaxRelErrSource(1));
    }
  }
}

void FPInstabilityAnalysis::pre_fcmp(SCOPE lScope, SCOPE rScope UNUSED, int64_t lValue, int64_t rValue UNUSED, KIND type UNUSED, PRED pred UNUSED, int line UNUSED, int inx UNUSED) {
  if (lScope != CONSTANT) {
    pre_analysis(lValue);
  }
}

void FPInstabilityAnalysis::post_fcmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type UNUSED, PRED pred, int line, int inx) {

  long double sv1, sv2; 
  int line1, line2; 
  bool sresult;

  sv1 = getShadowValue(lScope, lValue);
  sv2 = getShadowValue(rScope, rValue);

  line1 = getLineNumber(lScope, lValue);
  line2 = getLineNumber(rScope, rValue);

  switch(pred) {
    case CmpInst::FCMP_FALSE:
      sresult = 0;
      break;
    case CmpInst::FCMP_TRUE:
      sresult = 1;
      break;
    case CmpInst::FCMP_UEQ:
      sresult = sv1 == sv2;
      break;
    case CmpInst::FCMP_UNE:
      sresult = sv1 != sv2;
      break;
    case CmpInst::FCMP_UGT:
      sresult = sv1 > sv2;
      break;
    case CmpInst::FCMP_UGE:
      sresult = sv1 >= sv2;
      break;
    case CmpInst::FCMP_ULT:
      sresult = sv1 < sv2;
      break;
    case CmpInst::FCMP_ULE:
      sresult = sv1 <= sv2;
      break;
    case CmpInst::FCMP_OEQ:
      sresult = sv1 == sv2;
      break;
    case CmpInst::FCMP_ONE:
      sresult = sv1 != sv2;
      break;
    case CmpInst::FCMP_OGT:
      sresult = sv1 > sv2;
      break;
    case CmpInst::FCMP_OGE:
      sresult = sv1 >= sv2;
      break;
    case CmpInst::FCMP_OLT:
      sresult = sv1 < sv2;
      break;
    case CmpInst::FCMP_OLE:
      sresult = sv1 <= sv2;
      break;
    default:
      sresult = 0;
      DEBUG_STDERR("No support for FCMP predicate " << pred);
      safe_assert(false);
      break;
  }

  if (sresult != executionStack.top()[inx]->getIntValue()) {
    cerr << "[FPInstabilityAnalysis] Concrete difference at FCMP." << endl;
    DEBUG_STDERR("[FPInstabilityAnalysis] Concrete difference at FCMP.");
    DEBUG_STDERR("\tline: " << line);
    DEBUG_STDERR("\tshadow result = " << sresult << ", result = " << executionStack.top()[inx]->getIntValue());
    if (preFpISO != NULL) {
      FPInstabilityShadowObject<HIGHPRECISION> fpISO(*preFpISO);
      fpISO.setAbnormalType(FPInstabilityShadowObject<HIGHPRECISION>::CONCRETEDIFFERENCE);
      fpISO.setMaxRelErrSource(0, line1);
      fpISO.setMaxRelErrSource(1, line2);
      analysisTable[line] = fpISO;
      DEBUG_STDERR("\t sum relative error: " << preFpISO->getSumRelErr());
      DEBUG_STDERR("\t max relative error: " << preFpISO->getMaxRelErr());
      DEBUG_STDERR("\t max relative error source: " << preFpISO->getMaxRelErrSource(0) << ":" << preFpISO->getMaxRelErrSource(1));
    }
  }
}

void FPInstabilityAnalysis::pre_fptosi(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}

void FPInstabilityAnalysis::post_fptosi(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind, int size UNUSED, int inx) {

  double sv, cv;
  int64_t sresult;

  cv = getConcreteValue(opScope, op);
  sv = getShadowValue(opScope, op);

  switch (kind) {
    case INT1_KIND:
      sresult = (bool) sv;
      break;
    case INT8_KIND:
      sresult = (int8_t) sv;
      break;
    case INT16_KIND:
      sresult = (int16_t) sv;
      break;
    case INT24_KIND:
      sresult = (int32_t) sv;
      sresult = sresult & 0x00FFFFFF;
      break;
    case INT32_KIND:
      sresult = (int32_t) sv;
      break;
    case INT64_KIND:
      sresult = (int64_t) sv;
      break;
    default:
      sresult = 0;
      DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(kind));
      safe_assert(false);
  }

  if (sresult != executionStack.top()[inx]->getIntValue()) {
    DEBUG_STDERR("[FPInstabilityAnalysis] Concrete difference at FPTOSI.");
    DEBUG_STDERR("\tshadow operand = " << sv << ", concrete operand = " << cv);
    DEBUG_STDERR("\tshadow result = " << sresult << ", result = " << executionStack.top()[inx]->getIntValue());
    if (preFpISO != NULL) {
      FPInstabilityShadowObject<HIGHPRECISION> fpISO(*preFpISO);
      fpISO.setAbnormalType(FPInstabilityShadowObject<HIGHPRECISION>::CONCRETEDIFFERENCE);
      // TODO: add fpISO to analysis table
      DEBUG_STDERR("\t sum relative error: " << preFpISO->getSumRelErr());
      DEBUG_STDERR("\t max relative error: " << preFpISO->getMaxRelErr());
      DEBUG_STDERR("\t max relative error source: " << preFpISO->getMaxRelErrSource(0) << ":" << preFpISO->getMaxRelErrSource(1));
    }
  }
}

void FPInstabilityAnalysis::pre_fptrunc(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}

void FPInstabilityAnalysis::post_fptrunc(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind, int size UNUSED, int inx) {

  long double sv;
  long double sresult;
  FPInstabilityShadowObject<HIGHPRECISION> *shadow;

  sv = getShadowValue(opScope, op);

  switch (kind) {
    case FLP32_KIND:
      sresult = (float) sv;
      break;
    case FLP64_KIND:
      sresult = (double) sv;
      break;
    case FLP80X86_KIND:
      sresult = (long double) sv;
      break;
    default:
      sresult = 0;
      DEBUG_STDERR("Unsupported float type: " << KIND_ToString(kind));
      safe_assert(false);
  }

  if (preFpISO != NULL) {
    shadow = new FPInstabilityShadowObject<HIGHPRECISION>(0, 0, 0, BINOP_INVALID);
    preFpISO->copyTo(shadow);
    shadow->setValue(sresult);
    executionStack.top()[inx]->setShadow(shadow);
  }
}

void FPInstabilityAnalysis::pre_fpext(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
  if (opScope != CONSTANT) {
    pre_analysis(op);
  }
}

void FPInstabilityAnalysis::post_fpext(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind, int size UNUSED, int inx) {

  long double sv;
  long double sresult;
  FPInstabilityShadowObject<HIGHPRECISION> *shadow;

  sv = getShadowValue(opScope, op);

  switch (kind) {
    case FLP32_KIND:
      sresult = (float) sv;
      break;
    case FLP64_KIND:
      sresult = (double) sv;
      break;
    case FLP80X86_KIND:
      sresult = (long double) sv;
      break;
    default:
      sresult = 0;
      DEBUG_STDERR("Unsupported float type: " << KIND_ToString(kind));
      safe_assert(false);
  }

  if (preFpISO != NULL) {
    shadow = new FPInstabilityShadowObject<HIGHPRECISION>(0, 0, 0, BINOP_INVALID);
    preFpISO->copyTo(shadow);
    shadow->setValue(sresult);
    executionStack.top()[inx]->setShadow(shadow);
  }
}

void FPInstabilityAnalysis::post_analysis() {
  map<int, FPInstabilityShadowObject<HIGHPRECISION> >::iterator it;

  cerr << "" << endl;
  cerr << "#######################################################" << endl;
  cerr << "############## FPINSTABILITY ANALYSIS #################" << endl;
  cerr << "#######################################################" << endl;
  cerr << "" << endl;
  cerr << "[PARAMETER] Relative error threshold: 10^" << epsilon << endl;
  cerr << "[PARAMETER] Tracking from line: " << source << endl;
  cerr << "" << endl;
  cerr << "============= RECOMMENDATION" << endl;

  it = analysisTable.find(source);
  FPInstabilityShadowObject<HIGHPRECISION> fpISO = it->second;

  // if (it == analysisTable.end()) {
  if (fpISO.getMaxRelErr() < pow(10, epsilon)) {
    cerr << "[SUCCESS] No precision change required." << endl; 
  } else {
    queue<FPInstabilityShadowObject<HIGHPRECISION> > errorFpISO; // FPISO objects propagation trace
    set<int> highPrecision; // line that should performed in higher precision
    queue<int> errorTrace; // error propagation trace

    backwardAnalysis(source, highPrecision, pow(10, epsilon));

    //
    // Repeat until cannot propagate further backward
    //
    /*
       errorTrace.push(source); // source is the first error
       while (!errorTrace.empty()) {
       int line = errorTrace.front();
       errorTrace.pop();

       it = analysisTable.find(line);

       if (it != analysisTable.end()) {

    //
    // this line should be performed in higher precision
    //
    highPrecision.insert(line);

    FPInstabilityShadowObject<HIGHPRECISION> fpISO = it->second;
    errorFpISO.push(fpISO);
    analysisTable.erase(it);

    int line01 = fpISO.getMaxRelErrSource(0);
    int line02 = fpISO.getMaxRelErrSource(1); 

    if (analysisTable.find(line01) != analysisTable.end()) {
    errorTrace.push(line01);
    }

    if (line02 != line01 && analysisTable.find(line02) != analysisTable.end()) {
    errorTrace.push(line02);
    }
    }
    }
    */

    //
    // Print high level suggestion
    //
    for (set<int>::iterator it = highPrecision.begin(); it != highPrecision.end(); it++) {
      int line = *it;
      cerr << "[RECOMMENDATION] Result at line " << line << " should be computed in higher precision" << endl;
    }

    //
    // Print error traces in form of FPSIO objects
    //
    cerr << "" << endl;
    cerr << "============= Detailed Analysis" << endl;
    //    while (!errorFpISO.empty()) {
    //      FPInstabilityShadowObject<HIGHPRECISION> fpISO = errorFpISO.front();
    //      errorFpISO.pop();

    for (set<int>::iterator it = highPrecision.begin(); it != highPrecision.end(); it++) {
      int line = *it;
      FPInstabilityShadowObject<HIGHPRECISION> fpISO = analysisTable.find(line)->second;

      //
      // Print error messages
      //
      if (fpISO.getAbnormalType() != FPInstabilityShadowObject<HIGHPRECISION>::NONE) {
        if (fpISO.getAbnormalType() == FPInstabilityShadowObject<HIGHPRECISION>::CANCELLATION) {
          cerr << "[WARN] High relative error at location (line: " <<
            fpISO.getPC() << ", file: " << fpISO.getFileID() << ") , cbad: " <<
            fpISO.getMaxCBad() << endl;
          cerr << "\t binary operator: " << BINOP_ToString(fpISO.getBinOp()) << endl;
        } else if (fpISO.getAbnormalType() == FPInstabilityShadowObject<HIGHPRECISION>::CONCRETEDIFFERENCE) {
          cerr << "[WARN] Concrete difference at location: (line: " << fpISO.getPC() <<
            ", file:" << fpISO.getFileID() << ")" << endl;
          cerr << "\t source of concrete difference: (line: " << fpISO.getPC()
            << ", file:" << fpISO.getFileID() << ")" << endl;
        } else if (fpISO.getAbnormalType() == FPInstabilityShadowObject<HIGHPRECISION>::SYNC) {
          cerr << "[WARN] Assume high relative error from sync at location (line: " << 
            fpISO.getPC() << ", file: " << fpISO.getFileID() << ") , cbad: " <<
            fpISO.getMaxCBad() << endl;
        }

        cerr << "\t sum relative error: " << fpISO.getSumRelErr() << endl;
        cerr << "\t max relative error: " << fpISO.getMaxRelErr() << endl;
        cerr << "\t max relative error source: " << fpISO.getMaxRelErrSource(0)
          << ":" << fpISO.getMaxRelErrSource(1) << endl;
        cerr << "\t operand actual value 01 and 02: " <<
          fpISO.getMaxRelErrLowValue(0) << ":" <<
          fpISO.getMaxRelErrLowValue(1) << endl;
        cerr << "\t operand high value 01 and 02: " <<
          fpISO.getMaxRelErrHighValue(0) << ":" <<
          fpISO.getMaxRelErrHighValue(1) << endl;
        cerr << "\t value at max relative error: " << fpISO.getMaxRelErrValue() << endl;
        cerr << "\t max cbad:" << fpISO.getMaxCBad() << endl;
        cerr << "\t max cbad source:" << fpISO.getMaxCBadSource() << endl; 
      }
    }
  }

  /*
     for (map<int, FPInstabilityShadowObject<HIGHPRECISION> >::iterator it = analysisTable.begin(); it
     != analysisTable.end(); ++it) {
     int line = it->first;
     FPInstabilityShadowObject<HIGHPRECISION> fpISO = it->second;

     if (fpISO.getAbnormalType() != FPInstabilityShadowObject<HIGHPRECISION>::NONE) {
     if (fpISO.getAbnormalType() == FPInstabilityShadowObject<HIGHPRECISION>::CANCELLATION) {
     cerr << "[WARN] High relative error at location (line: " << fpISO.getPC() << ", file: " << fpISO.getFileID() << ") , cbad: " << fpISO.getMaxCBad() << endl;
     } else if (fpISO.getAbnormalType() == FPInstabilityShadowObject<HIGHPRECISION>::CONCRETEDIFFERENCE) {
     cerr << "[WARN] Concrete difference at location: (line: " << line << ", file:" << fpISO.getFileID() << ")" << endl;
     cerr << "\t source of concrete difference: (line: " << fpISO.getPC() << ", file:" << fpISO.getFileID() << ")" << endl;
     }

     cerr << "\t sum relative error: " << fpISO.getSumRelErr() << endl;
     cerr << "\t max relative error: " << fpISO.getMaxRelErr() << endl;
     cerr << "\t max relative error source: " << fpISO.getMaxRelErrSource(0) << ":" << fpISO.getMaxRelErrSource(1) << endl;
     cerr << "\t max cbad:" << fpISO.getMaxCBad() << endl;
     cerr << "\t max cbad source:" << fpISO.getMaxCBadSource() << endl; 
     }
     }
     */
  }

  void FPInstabilityAnalysis::backwardAnalysis(int line, set<int>& highPrecision, HIGHPRECISION errorThreshold) {
    map<int, FPInstabilityShadowObject<HIGHPRECISION> >::iterator it;

    it = analysisTable.find(line);
    safe_assert(it != analysisTable.end());

    FPInstabilityShadowObject<HIGHPRECISION> fpISO = it->second;

    if (fpISO.getMaxRelErr() >= errorThreshold) {
      highPrecision.insert(line); // the result need to be in higher precision

      BINOP op = fpISO.getBinOp();

      if (op != BINOP_INVALID) {
        //
        // Binop case:
        // Analyze constraint for two operands and invoke backwardAnalysis correspondingly.
        //
        int line01, line02;
        FPInstabilityShadowObject<HIGHPRECISION> fpISO1, fpISO2;

        line01 = fpISO.getMaxRelErrSource(0);
        line02 = fpISO.getMaxRelErrSource(1);

        safe_assert(line01 != -1);
        safe_assert(line02 != -1);

        it = analysisTable.find(line01);
        safe_assert(it != analysisTable.end());
        fpISO1 = it->second;
        it = analysisTable.find(line02);
        safe_assert(it != analysisTable.end());
        fpISO2 = it->second;

        //
        // Case 1: only the result need to be in higher precision
        //
        HIGHPRECISION result = eval(fpISO.getMaxRelErrLowValue(0), fpISO.getMaxRelErrLowValue(1), op);
        HIGHPRECISION err = computeRelativeError(fpISO.getValue(), result);

        if (err >= errorThreshold) {
          //
          // Case 2: the operands also need to be in higher precision
          //
          HIGHPRECISION subErrorThreshold;
          HIGHPRECISION high0, high1, high;

          high = fpISO.getMaxRelErrValue();
          high0 = fpISO.getMaxRelErrHighValue(0);
          high1 = fpISO.getMaxRelErrHighValue(1);
          switch (op) {
            case FADD:
            case FSUB:
              if (high == 0) {
                subErrorThreshold = 0; // TODO: recheck this
              } else {
                subErrorThreshold = errorThreshold * high / max(abs(high0-high1), abs(high0+high1));
              }
              break;
            case FMUL:
              if (high == 0) {
                subErrorThreshold = 0; // TODO: recheck this 
              } else {
                subErrorThreshold = errorThreshold / 2;
              }
              break;
            case FDIV:
              if (high == 0) {
                subErrorThreshold = 0; // TODO: recheck this
              } else {
                subErrorThreshold = errorThreshold / 2;
              }
              break;
            default:
              safe_assert(false);
              break;
          }

          if (highPrecision.find(line01) == highPrecision.end()) {
            backwardAnalysis(line01, highPrecision, subErrorThreshold);
          }
          if (highPrecision.find(line02) == highPrecision.end()) {
            backwardAnalysis(line02, highPrecision, subErrorThreshold);
          }
        } 
      } else {
        //
        // Not binop case:
        // Call backwardAnalysis on the maximum relative error source using the default epsilon.
        //
        highPrecision.insert(line); // (safely assume) the result need to be in higher precision

        int line01 = fpISO.getMaxRelErrSource(0);
        if (line01 != -1 && highPrecision.find(line01) == highPrecision.end()) {
          backwardAnalysis(line01, highPrecision, pow(10, epsilon));
        }
        int line02 = fpISO.getMaxRelErrSource(1);
        if (line02 != -1 && highPrecision.find(line02) == highPrecision.end()) {
          backwardAnalysis(line02, highPrecision, pow(10, epsilon));
        }
      }
    }
  }
