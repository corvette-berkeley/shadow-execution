#include "MoveAllocaInst.h"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include <vector>

using namespace llvm;
using namespace std;

bool MoveAllocaInst::runOnFunction(Function &F) {

  Function *f = &F;
  BasicBlock *entry = &f->getEntryBlock();
  vector<Instruction*> allocas;

  // collect allocas to move    
  for(Function::iterator b = f->begin(), be = f->end(); b != be; b++) {
    if (entry != b) {
      for(BasicBlock::iterator i = b->begin(), ie = b->end(); i != ie; i++) {
	if (AllocaInst* const allocaInst = dyn_cast<AllocaInst>(i)) {
	  allocas.push_back(allocaInst);
	}
      }
    }
  } // for

  // moving allocas
  for(unsigned i = 0; i < allocas.size(); i++) {
    allocas[i]->moveBefore(entry->getTerminator());
  }

  return true;
}


bool MoveAllocaInst::runOnModule(Module &M) {

  for(Module::iterator f = M.begin(), fe = M.end(); f != fe; f++) {
    if (!f->isDeclaration()) {
      runOnFunction(*f);
    }
  }

  return true;
}  


// same as current Rules
void MoveAllocaInst::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char MoveAllocaInst::ID = 0;
static const RegisterPass<MoveAllocaInst> registration("move-allocas", "Move alloca instructions to beginning of functions");
