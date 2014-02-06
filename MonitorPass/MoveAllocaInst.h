#ifndef MOVE_ALLOCA_INST_GUARD
#define MOVE_ALLOCA_INST_GUARD 1

#include <llvm/Pass.h>
#include <iostream>

namespace llvm {
  class Value;
}

using namespace std;
using namespace llvm;

class MoveAllocaInst : public ModulePass {

public:
  MoveAllocaInst() : ModulePass(ID) {}
  
  virtual bool runOnModule(Module &M);

  bool runOnFunction(Function &F);
  
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;

  static char ID; // Pass identification, replacement for typeid
};

#endif // MOVE_ALLOCA_INST_GUARD
