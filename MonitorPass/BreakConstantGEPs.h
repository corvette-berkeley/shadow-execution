//===- BreakConstantGEPs.h - Change constant GEPs into GEP instructions --- --//
//// 
////                          The SAFECode Compiler 
////
//// This file was developed by the LLVM research group and is distributed under
//// the University of Illinois Open Source License. See LICENSE.TXT for details.
//// 
////===----------------------------------------------------------------------===//
////
//// This pass changes all GEP constant expressions into GEP instructions.  This
//// permits the rest of SAFECode to put run-time checks on them if necessary.
////
////===----------------------------------------------------------------------===//

#ifndef BREAKCONSTANTGEPS_H
#define BREAKCONSTANTGEPS_H

#include "llvm/Pass.h"

namespace llvm {
  class Value;
}

using namespace std;
using namespace llvm;

//
//// Pass: BreakConstantGEPs
////
//// Description:
////  This pass modifies a function so that it uses GEP instructions instead of
////  GEP constant expressions.
////
class BreakConstantGEPs : public FunctionPass {

  public:
    static char ID;

    BreakConstantGEPs() : FunctionPass(ID) {}

    const char *getPassName() const { return "Remove Constant GEP Expressions";}

    virtual bool runOnFunction (Function & F);

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      // This pass does not modify the control-flow graph of the function
      AU.setPreservesCFG();
    }
};

#endif
