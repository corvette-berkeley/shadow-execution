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
#include <llvm/IR/InstrTypes.h>

/******** HELPER FUNCTIONS **********/

void FPInstabilityAnalysis::copyShadow(IValue *src, IValue *dest) {
  //
  // copy shadow value from src to dest
  //
  if (src->isFlpValue() && dest->isFlpValue() && src->getShadow() != NULL) {
    FPInstabilityShadowObject *fpISOSrc, *fpISODest; 
    
    fpISOSrc = (FPInstabilityShadowObject *) src->getShadow();
    fpISODest = new FPInstabilityShadowObject(0,0);
    fpISOSrc->copyTo(fpISODest);

    dest->setShadow(fpISODest);
  }
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
        ((FPInstabilityShadowObject*) iv->getShadow())->getValue();
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
    line = iv->getLineNumber();
  }

  return line;
}

int FPInstabilityAnalysis::getEBits(long double v1, long double v2) {
  int p = 32768; // exponent of long double, 2^15 

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

void FPInstabilityAnalysis::fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx, BINOP op) {
  InterpreterObserver::binop(lScope, rScope, lValue, rValue, type, line, inx, op);

  long double v1, v2, sv1, sv2, sresult, result;
  FPInstabilityShadowObject *fpISO;
  long double rerr, d;
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
  if (executionStack.top()[inx]->getShadow() == NULL) {
    fpISO = new FPInstabilityShadowObject(sresult, line);
  } else {
    fpISO = (FPInstabilityShadowObject*) executionStack.top()[inx]->getShadow();
  }

  //
  // Compute cancellation badness and relative errors.
  // See: "A dyanmic program analysis to find floating-point accuracy problems"
  // by F. Benz, A. Hildebrandt and S. Hack, PLDO 2012, p.6
  //
  result = executionStack.top()[inx]->getFlpValue();
  d = sresult != 0 ? sresult : 10e-10;
  rerr = abs((long double)((sresult - result)/d));

  fpISO->setSumRelErr(fpISO->getSumRelErr() + rerr);

  if (rerr > fpISO->getMaxRelErr()) {
    fpISO->setMaxRelErr(rerr);
    fpISO->setMaxRelErrSource(0, l1);
    fpISO->setMaxRelErrSource(1, l2);
  }

  e1 = getExponent(v1);
  e2 = getExponent(v2);
  e3 = getExponent(result);
  cbits = max(e1, e2) - e3;
  cbad = max(0, 1 + cbits - min(getEBits(sv1, v1), getEBits(sv2, v2)));
  fpISO->setSumCBad(fpISO->getSumCBad() + cbad);
  if (cbad > fpISO->getMaxCBad()) {
    fpISO->setMaxCBad(cbad);
    fpISO->setMaxCBadSource(line);
  }

  fpISO->setValue(sresult);
  fpISO->setPC(line);

  if (cbad > 0) {
    cerr << "[WARN] Catastrophic cancellation at line: " << line << ", cbad: " << cbad << endl;
    cerr << "\t binop:" << BINOP_ToString(op) << endl;
    cerr << "\t v1:" << v1 << ", sv1:" << sv1 << endl;
    cerr << "\t v2:" << v2 << ", sv2:" << sv2 << endl;
    cerr << "\t result:" << result << ", sresult:" << sresult << endl;
  }

  executionStack.top()[inx]->setShadow(fpISO);
}

void FPInstabilityAnalysis::fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  fbinop(lScope, rScope, lValue, rValue, type, line, inx, FADD);
}

void FPInstabilityAnalysis::fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  fbinop(lScope, rScope, lValue, rValue, type, line, inx, FSUB);
}

void FPInstabilityAnalysis::fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  fbinop(lScope, rScope, lValue, rValue, type, line, inx, FMUL);
}

void FPInstabilityAnalysis::fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
  fbinop(lScope, rScope, lValue, rValue, type, line, inx, FDIV);
}

void FPInstabilityAnalysis::create_global_symbol_table(int size) {
  InterpreterObserver::create_global_symbol_table(size);

  //
  // instantiate copyShadow
  //
  IValue::setCopyShadow(&copyShadow);
}

/*

void FPInstabilityAnalysis::fptoui(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  InterpreterObserver::fptoui(iid, type, op, size, inx);

  long double sv;
  int64_t sresult;

  if (op->inx == -1) {
    sv = (long double) op->value.as_flp;
  } else {
    IValue* iv;

    iv = op->isGlobal ? globalSymbolTable[op->inx] :
      executionStack.top()[op->inx];
    sv = iv->getShadow() == NULL ? (long double) iv->getFlpValue() : *((long
          double*) iv->getShadow());
  }

  switch (type) {
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
      DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
      safe_assert(false);
  }

  if (sresult != executionStack.top()[inx]->getIntValue()) {
    cerr << "[FPInstabilityAnalysis] Concrete difference at FPTOUI." << endl; 
  }
}

void FPInstabilityAnalysis::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  InterpreterObserver::fcmp(iid, op1, op2, pred, inx);

  long double sv1, sv2; 
  bool sresult;

  if (op1->inx == -1) {
    sv1 = (long double) KVALUE_ToFlpValue(op1);
  } else {
    IValue* iv1;

    iv1 = op1->isGlobal ? globalSymbolTable[op1->inx] :
      executionStack.top()[op1->inx];
    sv1 = iv1->getShadow() == NULL ? (long double) iv1->getFlpValue() : *((long
          double*) iv1->getShadow());
  }

  if (op2->inx == -1) {
    sv2 = (long double) KVALUE_ToFlpValue(op2);
  } else {
    IValue* iv2;

    iv2 = op2->isGlobal ? globalSymbolTable[op2->inx] :
      executionStack.top()[op2->inx];
    sv2 = iv2->getShadow() == NULL ? (long double) iv2->getFlpValue() : *((long
          double*) iv2->getShadow());
  }

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
  }
}

void FPInstabilityAnalysis::fptosi(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  InterpreterObserver::fptosi(iid, type, op, size, inx);

  long double sv;
  int64_t sresult;

  if (op->inx == -1) {
    sv = (long double) op->value.as_flp;
  } else {
    IValue* iv;

    iv = op->isGlobal ? globalSymbolTable[op->inx] :
      executionStack.top()[op->inx];
    sv = iv->getShadow() == NULL ? (long double) iv->getFlpValue() : *((long
          double*) iv->getShadow());
  }

  switch (type) {
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
    case INT32_KIND:
      sresult = (int32_t) sv;
      break;
    case INT64_KIND:
      sresult = (int64_t) sv;
      break;
    default:
      sresult = 0;
      DEBUG_STDERR("Unsupported integer type: " << KIND_ToString(type));
      safe_assert(false);
  }

  if (sresult != executionStack.top()[inx]->getIntValue()) {
    cerr << "[FPInstabilityAnalysis] Concrete difference at FPTOUI." << endl; 
  }
}

void FPInstabilityAnalysis::fptrunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  InterpreterObserver::fptrunc(iid, type, op, size, inx);

  long double sv;
  long double sresult;
  long double *shadow;

  if (op->inx == -1) {
    sv = (long double) op->value.as_flp;
  } else {
    IValue* iv;

    iv = op->isGlobal ? globalSymbolTable[op->inx] :
      executionStack.top()[op->inx];
    sv = iv->getShadow() == NULL ? (long double) iv->getFlpValue() : *((long
          double*) iv->getShadow());
  }

  switch (type) {
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
      DEBUG_STDERR("Unsupported float type: " << KIND_ToString(type));
      safe_assert(false);
  }

  shadow = (long double *) malloc(sizeof(long double));
  shadow[0] = sresult;
  executionStack.top()[inx]->setShadow(shadow);
}

void FPInstabilityAnalysis::fpext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  InterpreterObserver::fpext(iid, type, op, size, inx);

  long double sv;
  long double sresult;
  long double *shadow;

  if (op->inx == -1) {
    sv = (long double) op->value.as_flp;
  } else {
    IValue* iv;

    iv = op->isGlobal ? globalSymbolTable[op->inx] :
      executionStack.top()[op->inx];
    sv = iv->getShadow() == NULL ? (long double) iv->getFlpValue() : *((long
          double*) iv->getShadow());
  }

  switch (type) {
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
      DEBUG_STDERR("Unsupported float type: " << KIND_ToString(type));
      safe_assert(false);
  }

  shadow = (long double *) malloc(sizeof(long double));
  shadow[0] = sresult;
  executionStack.top()[inx]->setShadow(shadow);
}

*/
