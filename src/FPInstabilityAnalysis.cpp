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

#include "FPInstabilityAnalysis.h"
#include <llvm/IR/InstrTypes.h>

void FPInstabilityAnalysis::copy(IValue* src, IValue* dest) {
  InterpreterObserver::copy(src, dest);

  //
  // copy shadow value (a long double) from src to dest
  //
  if (src->isFlpValue() && dest->isFlpValue()) {
    long double *shadow = (long double *) malloc(sizeof(long double));
    shadow[0] = *((long double *) src->getShadow());
    dest->setShadow(shadow);
  }
}

void FPInstabilityAnalysis::fbinop(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx, BINOP op) {
  long double sv1, sv2, sresult;

  InterpreterObserver::binop(iid, nuw, nsw, op1, op2, inx, op);

  if (op1->inx == -1) {
    sv1 = (long double) op1->value.as_flp;
  } else {
    IValue* iv1;

    iv1 = op1->isGlobal ? globalSymbolTable[op1->inx] :
      executionStack.top()[op1->inx];
    sv1 = iv1->getShadow() == NULL ? (long double) iv1->getFlpValue() : *((long
          double*) iv1->getShadow());
  }

  if (op2->inx == -1) {
    sv2 = (long double) op2->value.as_flp;
  } else {
    IValue* iv2;

    iv2 = op2->isGlobal ? globalSymbolTable[op2->inx] :
      executionStack.top()[op2->inx];
    sv2 = iv2->getShadow() == NULL ? (long double) iv2->getFlpValue() : *((long
          double*) iv2->getShadow());
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

  long double *shadow = (long double *) malloc(sizeof(long double));
  shadow[0] = sresult;

  /*
  cout << "[FBINOP]" << endl;
  cout << "interpreted value: " << executionStack.top()[inx]->getFlpValue() << endl;;
  cout << "shadow value: " << sresult << endl;;
  */

  executionStack.top()[inx]->setShadow(shadow);
}

void FPInstabilityAnalysis::fadd(IID id, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  fbinop(id, nuw, nsw, op1, op2, inx, FADD);
}

void FPInstabilityAnalysis::fsub(IID id, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  fbinop(id, nuw, nsw, op1, op2, inx, FSUB);
}

void FPInstabilityAnalysis::fmul(IID id, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  fbinop(id, nuw, nsw, op1, op2, inx, FMUL);
}

void FPInstabilityAnalysis::fdiv(IID id, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  fbinop(id, nuw, nsw, op1, op2, inx, FDIV);
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

  /*
  cerr << "[FCMP] << endl;
  cerr << "interpreted 1: " << v1 << endl;
  cerr << "interpreted 2: " << v2 << endl;
  cerr << "shadow 1: " << sv1 << endl;
  cerr << "shadow 2: " << sv2 << endl; 
  */

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
