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

void BlameTree::post_create_global_symbol_table() {
  //
  // instantiate copyShadow
  //
  IValue::setCopyShadow(&copyShadow);
}

void BlameTree::setShadowObject(SCOPE scope, int64_t inx, BlameTreeShadowObject<HIGHPRECISION>* shadowObject) {

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

/******* ANALYSIS FUNCTIONS *******/

void BlameTree::post_call_sin(IID iid UNUSED, bool nounwind UNUSED, int pc, KIND type UNUSED, int inx, SCOPE argScope, int64_t argValueOrIndex) {
  BlameTreeShadowObject<HIGHPRECISION> *shadow;
  LOWPRECISION arg, result;
  HIGHPRECISION sarg, sresult;
  PRECISION p;

  //
  // Obtain actual values and shadow values
  //
  arg = getActualValue(argScope, argValueOrIndex);
  result = executionStack.top()[inx]->getFlpValue();

  shadow = getShadowObject(argScope, argValueOrIndex);

  //
  // Perform sin function on shadow values
  // 
  HIGHPRECISION values[PRECISION_NO];

  // shadow object for operands
  if (!shadow) {
    // constructing and setting shadow object
    HIGHPRECISION values[PRECISION_NO];
    PRECISION p;
    values[BITS_FLOAT] = arg;
    values[BITS_DOUBLE] = (HIGHPRECISION) arg;
    for (p = PRECISION(BITS_FLOAT+1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
      values[p] = BlameTreeUtilities::clearBits((HIGHPRECISION) arg, 52 -
          BlameTreeUtilities::exactBits(p));
    }
    
    shadow = new BlameTreeShadowObject<HIGHPRECISION>(pc, dynamicCounter, CONSTANT_INTR,
        BINOP_INVALID, "NONE", values);
    setShadowObject(argScope, argValueOrIndex, shadow);
  }


  // retrieve the value in higher precision
  sarg = shadow->getValue(BITS_DOUBLE);
  sresult = sin(sarg);

  values[BITS_FLOAT] = result;
  values[BITS_DOUBLE] = sresult;
  for (p = PRECISION(BITS_FLOAT + 1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
    values[p] = BlameTreeUtilities::clearBits(sresult, 52 -
        BlameTreeUtilities::exactBits(p));
  }

  // creating shadow object for the result
  BlameTreeShadowObject<HIGHPRECISION> *resultShadow = new
    BlameTreeShadowObject<HIGHPRECISION>(pc, dynamicCounter, CALL_INTR, BINOP_INVALID, "sin",
        values); 
  executionStack.top()[inx]->setShadow(resultShadow);

  // adding to the trace
  trace[dynamicCounter].push_back(*resultShadow);
  trace[dynamicCounter].push_back(*shadow);

  dynamicCounter++; 
  return;
}

void BlameTree::post_call_acos(IID iid UNUSED, bool nounwind UNUSED, int pc, KIND type UNUSED, int inx, SCOPE argScope, int64_t argValueOrIndex) {
  BlameTreeShadowObject<HIGHPRECISION> *shadow;
  LOWPRECISION arg, result;
  HIGHPRECISION sarg, sresult;
  PRECISION p;

  //
  // Obtain actual values and shadow values
  //
  arg = getActualValue(argScope, argValueOrIndex);
  result = executionStack.top()[inx]->getFlpValue();

  shadow = getShadowObject(argScope, argValueOrIndex);

  //
  // Perform acos function on shadow values
  // 
  HIGHPRECISION values[PRECISION_NO];

  // shadow object for operands
  if (!shadow) {
    // constructing and setting shadow object
    HIGHPRECISION values[PRECISION_NO];
    PRECISION p;
    values[BITS_FLOAT] = arg;
    values[BITS_DOUBLE] = (HIGHPRECISION) arg;
    for (p = PRECISION(BITS_FLOAT+1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
      values[p] = BlameTreeUtilities::clearBits((HIGHPRECISION) arg, 52 -
          BlameTreeUtilities::exactBits(p));
    }
    
    shadow = new BlameTreeShadowObject<HIGHPRECISION>(pc, dynamicCounter, CONSTANT_INTR,
        BINOP_INVALID, "NONE", values);
    setShadowObject(argScope, argValueOrIndex, shadow);
  }


  // retrieve the value in higher precision
  sarg = shadow->getValue(BITS_DOUBLE);
  sresult = acos(sarg);

  values[BITS_FLOAT] = result;
  values[BITS_DOUBLE] = sresult;
  for (p = PRECISION(BITS_FLOAT + 1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
    values[p] = BlameTreeUtilities::clearBits(sresult, 52 -
        BlameTreeUtilities::exactBits(p));
  }

  // creating shadow object for the result
  BlameTreeShadowObject<HIGHPRECISION> *resultShadow = new
    BlameTreeShadowObject<HIGHPRECISION>(pc, dynamicCounter, CALL_INTR, BINOP_INVALID, "acos",
        values); 
  executionStack.top()[inx]->setShadow(resultShadow);

  // adding to the trace
  trace[dynamicCounter].push_back(*resultShadow);
  trace[dynamicCounter].push_back(*shadow);

  dynamicCounter++; 
  return;
}

void BlameTree::post_call_sqrt(IID iid UNUSED, bool nounwind UNUSED, int pc, KIND type UNUSED, int inx, SCOPE argScope, int64_t argValueOrIndex) {
  BlameTreeShadowObject<HIGHPRECISION> *shadow;
  LOWPRECISION arg, result;
  HIGHPRECISION sarg, sresult;
  PRECISION p;

  //
  // Obtain actual values and shadow values
  //
  arg = getActualValue(argScope, argValueOrIndex);
  result = executionStack.top()[inx]->getFlpValue();

  shadow = getShadowObject(argScope, argValueOrIndex);

  //
  // Perform sqrt function on shadow values
  // 
  HIGHPRECISION values[PRECISION_NO];

  // shadow object for operands
  if (!shadow) {
    // constructing and setting shadow object
    HIGHPRECISION values[PRECISION_NO];
    PRECISION p;
    values[BITS_FLOAT] = arg;
    values[BITS_DOUBLE] = (HIGHPRECISION) arg;
    for (p = PRECISION(BITS_FLOAT+1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
      values[p] = BlameTreeUtilities::clearBits((HIGHPRECISION) arg, 52 -
          BlameTreeUtilities::exactBits(p));
    }
    
    shadow = new BlameTreeShadowObject<HIGHPRECISION>(pc, dynamicCounter, CONSTANT_INTR,
        BINOP_INVALID, "NONE", values);
    setShadowObject(argScope, argValueOrIndex, shadow);
  }


  // retrieve the value in higher precision
  sarg = shadow->getValue(BITS_DOUBLE);
  sresult = sqrt(sarg);

  values[BITS_FLOAT] = result;
  values[BITS_DOUBLE] = sresult;
  for (p = PRECISION(BITS_FLOAT + 1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
    values[p] = BlameTreeUtilities::clearBits(sresult, 52 -
        BlameTreeUtilities::exactBits(p));
  }

  // creating shadow object for the result
  BlameTreeShadowObject<HIGHPRECISION> *resultShadow = new
    BlameTreeShadowObject<HIGHPRECISION>(pc, dynamicCounter, CALL_INTR, BINOP_INVALID, "sqrt",
        values); 
  executionStack.top()[inx]->setShadow(resultShadow);

  // adding to the trace
  trace[dynamicCounter].push_back(*resultShadow);
  trace[dynamicCounter].push_back(*shadow);

  dynamicCounter++; 
  return;
}

void BlameTree::post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue,
    int64_t rValue, KIND type, int line UNUSED, int inx UNUSED, BINOP op) {

  BlameTreeShadowObject<HIGHPRECISION> *s1, *s2;
  HIGHPRECISION sv1, sv2, sresult;
  LOWPRECISION v1, v2;
  PRECISION i;

  //
  // assert: type is a floating-point type
  //
  safe_assert(type == FLP32_KIND || type == FLP64_KIND || type == FLP80X86_KIND
      || type == FLP128_KIND || type == FLP128PPC_KIND);

  //
  // Obtain actual values, and shadow values
  //
  v1 = getActualValue(lScope, lValue);
  v2 = getActualValue(rScope, rValue);
  
  s1 = getShadowObject(lScope, lValue);
  s2 = getShadowObject(rScope, rValue);

  //
  // Perform binary operation for shadow values
  //
  HIGHPRECISION values[PRECISION_NO];

  // shadow objects for operands
  if (!s1) {
    // constructing and setting shadow object
    HIGHPRECISION values[PRECISION_NO];
    PRECISION p;
    values[BITS_FLOAT] = v1;
    values[BITS_DOUBLE] = (HIGHPRECISION) v1;
    for (p = PRECISION(BITS_FLOAT+1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
      values[p] = BlameTreeUtilities::clearBits((HIGHPRECISION) v1, 52 -
          BlameTreeUtilities::exactBits(p));
    }

    s1 = new BlameTreeShadowObject<HIGHPRECISION>(line, dynamicCounter, CONSTANT_INTR,
        BINOP_INVALID, "NONE", values);
    setShadowObject(lScope, lValue, s1);
  }

  if (!s2) {
    // constructing and setting shadow object
    HIGHPRECISION values[PRECISION_NO];
    PRECISION p;
    values[BITS_FLOAT] = v2;
    values[BITS_DOUBLE] = (HIGHPRECISION) v2;
    for (p = PRECISION(BITS_FLOAT+1); p < BITS_DOUBLE; p = PRECISION(p+1)) {
      values[p] = BlameTreeUtilities::clearBits((HIGHPRECISION) v2, 52 -
          BlameTreeUtilities::exactBits(p));
    }

    s2 = new BlameTreeShadowObject<HIGHPRECISION>(line, dynamicCounter, CONSTANT_INTR,
        BINOP_INVALID, "NONE", values);
    setShadowObject(rScope, rValue, s2);
  }

  // retrieve values in higher precision
  sv1 = s1->getValue(BITS_DOUBLE);
  sv2 = s2->getValue(BITS_DOUBLE);

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

  // truncate result depending on precision
  values[BITS_FLOAT] = executionStack.top()[inx]->getFlpValue();
  values[BITS_DOUBLE] = sresult;
  for (i = PRECISION(BITS_FLOAT+1); i < BITS_DOUBLE; i = PRECISION(i+1)) {
    values[i] = BlameTreeUtilities::clearBits(sresult, 52 -
        BlameTreeUtilities::exactBits(i));
  }

  // creating shadow object for target
  BlameTreeShadowObject<HIGHPRECISION> *resultShadow = 
    new BlameTreeShadowObject<HIGHPRECISION>(line, dynamicCounter, 
        BIN_INTR, op, "NONE", values);
  executionStack.top()[inx]->setShadow(resultShadow);

  // adding to the trace
  trace[dynamicCounter].push_back(*resultShadow);
  trace[dynamicCounter].push_back(*s1);
  trace[dynamicCounter].push_back(*s2);

  dynamicCounter++;
  return;
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

void BlameTree::post_fptrunc(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx) {
  BlameTreeShadowObject<HIGHPRECISION> *opbtSO = getShadowObject(opScope, op);

  if (opbtSO) {
    BlameTreeShadowObject<HIGHPRECISION> *btSO = new BlameTreeShadowObject<HIGHPRECISION>(*opbtSO);
    executionStack.top()[inx]->setShadow(btSO);
  }
}

void BlameTree::post_fpext(int64_t op, SCOPE opScope, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx) {
  BlameTreeShadowObject<HIGHPRECISION> *opbtSO = getShadowObject(opScope, op);

  if (opbtSO) {
    BlameTreeShadowObject<HIGHPRECISION> *btSO = new BlameTreeShadowObject<HIGHPRECISION>(*opbtSO);
    executionStack.top()[inx]->setShadow(btSO);
  }
}

void BlameTree::post_analysis() {
  cout << "Printing trace after analysis: " << dynamicCounter << endl;
  cout << "Trace length: " << dynamicCounter << endl;
  std::string line;

  //
  // Print the trace 
  //
  cout << endl;
  cout << "Do you want to see the trace (yes|no)?" << endl;
  std::getline(std::cin, line);

  if (line.compare("yes") == 0) {
    for(int i = max(0, dynamicCounter-500); i < dynamicCounter; i++) {
      cout << "DPC: " << i << endl;
      for(unsigned j = 0; j < trace[i].size(); j++) {
        cout << "\t";
        trace[i][j].print();
      }
    }
  }

  //
  // read rootnode from a file
  //
  cout << endl;
  cout << "Ok, now you have seen the trace, take a look at all computation points with values with different precision." << endl;
  cout << "Tell me:" << endl;
  cout << "\t Which computation point you are interested in?" << endl;
  cout << "\t What is your desired precision for that computation point?" << endl;

  cout << endl;
  cout << "I suggest the following parameter" << endl;
  cout << "\t Computation point: " << (dynamicCounter - 1) << endl;
  cout << "\t Desired precision: exact to " <<
    BlameTreeUtilities::precisionToString(PRECISION(PRECISION_NO/2)) << endl; 

  BlameNodeID rootNode(0,BITS_FLOAT);
  std::getline(std::cin, line);

  if (line.empty()) {
    rootNode = BlameNodeID(dynamicCounter-1, PRECISION(PRECISION_NO/2));
  } else {
    std::stringstream lineStream(line);
    std::string token;
    int dpc, prec; 

    lineStream >> token; 
    dpc = atoi(token.c_str());
    lineStream >> token; 
    prec = atoi(token.c_str());

    rootNode = BlameNodeID(dpc, BlameTreeUtilities::exactBitToPrecision(prec));
  }

  //
  // construct blame tree
  //
  cout << endl;
  cout << "Constructing blame tree from root at:" << endl; 
  cout << "\tComputation point: " <<  rootNode.getDPC() << endl; 
  cout << "\tDesired precision: " << BlameTreeUtilities::precisionToString(rootNode.getPrecision()) << endl;

  BlameTreeAnalysis bta(rootNode);
  clock_t startTime, endTime;

  startTime = clock();

  bta.constructBlameGraph(trace);

  endTime = clock();

  cout << endl;
  cout << "Done!" << endl;
  cout << "Construction time: " << double(endTime-startTime)/double(CLOCKS_PER_SEC) << " seconds." << endl;
  cout << "Analysis result:" << endl;

  bta.printResult();

  cout << endl;
  cout << "Do you want to visualize the blame tree? (yes|no)" << endl;
  std::getline(std::cin, line);

  if (line.compare("yes") == 0) {
    cout << endl;
    cout << "Blame tree in dot format: blametree.dot" << endl;
    cout << endl;

    ofstream blametree;
    blametree.open("blametree.dot");
    blametree << bta.toDot();
    blametree.close();
  }
  
  return;
}
