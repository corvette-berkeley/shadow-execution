#define DEBUG_TYPE "hello"

#include "Instrumentation.h"
#include "Instrumenter.h"
#include "MonitorPass.h"

namespace {

struct MonitorPass : public FunctionPass {
  static char ID;

  MonitorPass() : FunctionPass(ID) {}
  ~MonitorPass() {}

  virtual bool runOnFunction(Function &F) {
    Module* M = F.getParent();

    Instrumentation* instrumentation = Instrumentation::GetInstance();
    safe_assert(instrumentation != NULL);

    /* 
     * First pass through the function body to compute varCount and indices     
     * i) varCount: the number of local variables and registers                 
     * ii) indices: a mapping from each local variable and register to an index 
     */

    // set up varCount and indices map
    instrumentation->BeginFunction();

    // create index for all instructions
    for (Function::iterator BB = F.begin(), e = F.end(); BB != e; ++BB) {
      BasicBlock* block = (BasicBlock*) BB;
      IID blockIID = static_cast<IID>(reinterpret_cast<ADDRINT>(block));
      instrumentation->createBlockIndex(blockIID);
      for (BasicBlock::iterator itr = BB->begin(), end = BB->end(); itr != end; ++itr) {
        Instruction* inst = (Instruction*) itr;
        IID iid = static_cast<IID>(reinterpret_cast<ADDRINT>(inst));
        instrumentation->createIndex(iid);
      }
    }

    /* 
     * Second pass through the function body for instrumentation                
     * i) insert call to llvm_create_stack_frame at the beginning of each       
     *    function                                                              
     * ii) instrument each instruction using instrumentation->CheckAndInstrument 
     */

    unsigned int skip = 0;
    bool isFirstInstruction = true; 
    bool isFirstBlockInstruction = true;

    for (Function::iterator BB = F.begin(), e = F.end(); BB != e; ++BB) {
      // set up pointers to BB, F, and M
      instrumentation->BeginBasicBlock(BB, &F, M);
      isFirstBlockInstruction = true;
      for (BasicBlock::iterator itr = BB->begin(), end = BB->end(); itr != end; ++itr) {

        BasicBlock* block = (BasicBlock*) BB;

        if (isFirstInstruction) {
          // insert a call to create stack frame
          Constant* frameSize =
            ConstantInt::get(Type::getInt32Ty(instrumentation->M_->getContext()),
                instrumentation->getFrameSize(), SIGNED);
          TypePtrVector argTypes;
          argTypes.push_back(Type::getInt32Ty(instrumentation->M_->getContext()));

          ValuePtrVector args;
          args.push_back(frameSize);

          FunctionType* funType =
            FunctionType::get(Type::getVoidTy(instrumentation->M_->getContext()),
                ArrayRef<Type*>(argTypes), false);
          Instruction* call =
            CallInst::Create(instrumentation->M_->getOrInsertFunction(
                  StringRef("llvm_create_stack_frame"), funType),
                ArrayRef<Value*>(args)); 
          call->insertBefore(itr);
          isFirstInstruction = false;

	  // if this is the first block of main, skip it all
	  /*
	  if (F.getName() == "main") {
	    skip = block->size() - 1;
	    cout << "Setting skip to: " << skip << endl;
	  }
	  */
        } 

        if (isFirstBlockInstruction) {
          // insert a call to record block id 
          Constant* blockId =
            ConstantInt::get(Type::getInt32Ty(instrumentation->M_->getContext()),
                instrumentation->getBlockIndex(block), SIGNED);
          TypePtrVector argTypes;
          argTypes.push_back(Type::getInt32Ty(instrumentation->M_->getContext()));

          ValuePtrVector args;
          args.push_back(blockId);

          FunctionType* funType =
            FunctionType::get(Type::getVoidTy(instrumentation->M_->getContext()),
                ArrayRef<Type*>(argTypes), false);
          Instruction* call =
            CallInst::Create(instrumentation->M_->getOrInsertFunction(
                  StringRef("llvm_record_block_id"), funType),
                ArrayRef<Value*>(args)); 

          if (dyn_cast<PHINode>(itr)) {
            call->insertAfter(itr);
          } else {
            call->insertBefore(itr);
          }

          isFirstBlockInstruction = false;	   
        }

        if (skip == 0) {

          if (instrumentation->CheckAndInstrument(itr)) {

            if (CallInst* callInst = dyn_cast<CallInst>(itr)) {
              if (callInst->getCalledFunction() != NULL &&
                  callInst->getCalledFunction()->getName() != "malloc") {
                bool noUnwind =
                  callInst->getAttributes().hasAttrSomewhere(Attribute::NoUnwind)
                  || (dyn_cast<IntrinsicInst>(callInst) != NULL &&
                      !callInst->getType()->isVoidTy());
                if (noUnwind) {
                  skip = 12;
                }
              }

            } else if (dyn_cast<PHINode>(itr)) {
              skip = 2;
            }

          }

        }
        else {
          skip--;
        }

      }
    }
    return true;
  }

  bool doInitialization(Module &M) {
    /*
   Instrumentation* instrumentation = Instrumentation::GetInstance();
   instrumentation->BeginModule(&M);
   Instrumenter* instrumenter = new Instrumenter("", instrumentation);
   
   // split first block
   Function* mainFunction = M.getFunction("main");
   BasicBlock* firstBlock = &mainFunction->getEntryBlock();
   firstBlock->splitBasicBlock(firstBlock->begin(), "");
   
   cout << "=======Iterating through globals" << endl;
   for(Module::global_iterator i = M.global_begin(), e = M.global_end(); i != e; i++) {    
     if (!GlobalValue::isPrivateLinkage(i->getLinkage())) {
       i->dump();
       
       InstrPtrVector instrs;
       ValuePtrVector args;
       Value* global = instrumenter->KVALUE_VALUE(i, instrs, NOSIGN);
       args.push_back(global);
       
       TypePtrVector argTypes;
	argTypes.push_back(instrumenter->KVALUEPTR_TYPE());
	
	Instruction* call = instrumenter->CALL_INSTR("llvm_create_global", instrumenter->VOID_FUNC_TYPE(argTypes), args); // firstBlock
	instrs.push_back(call);
	
	instrumenter->InsertAllBefore(instrs, firstBlock->getTerminator());
     }
    }
   
   mainFunction->dump();
   */
   return Instrumentation::GetInstance()->Initialize(M);
  }

  bool doFinalization(Module &M) {
    return Instrumentation::GetInstance()->Finalize(M);
  }

  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
  };
};

/*******************************************************************************************/

} // end namespace

/*******************************************************************************************/

#include "StoreInstrumenter.h"
#include "LoadInstrumenter.h"
#include "AddInstrumenter.h"
#include "SubInstrumenter.h"
#include "MulInstrumenter.h"
#include "UDivInstrumenter.h"
#include "SDivInstrumenter.h"
#include "URemInstrumenter.h"
#include "SRemInstrumenter.h"
#include "FAddInstrumenter.h"
#include "FSubInstrumenter.h"
#include "FMulInstrumenter.h"
#include "FDivInstrumenter.h"
#include "FRemInstrumenter.h"
#include "ShlInstrumenter.h"
#include "LShrInstrumenter.h"
#include "AShrInstrumenter.h"
#include "AndInstrumenter.h"
#include "OrInstrumenter.h"
#include "XorInstrumenter.h"
#include "CallInstrumenter.h"
#include "GetElementPtrInstrumenter.h"
#include "BitCastInstrumenter.h"
#include "TruncInstrumenter.h"
#include "FPExtInstrumenter.h"
#include "FPToSIInstrumenter.h"
#include "FPToUIInstrumenter.h"
#include "FPTruncInstrumenter.h"
#include "IntToPtrInstrumenter.h"
#include "PtrToIntInstrumenter.h"
#include "SExtInstrumenter.h"
#include "SIToFPInstrumenter.h"
#include "TruncInstrumenter.h"
#include "UIToFPInstrumenter.h"
#include "ZExtInstrumenter.h"
#include "ExtractElementInstrumenter.h"
#include "InsertElementInstrumenter.h"
#include "ShuffleVectorInstrumenter.h"
#include "ExtractValueInstrumenter.h"
#include "InsertValueInstrumenter.h"
#include "UnreachableInstrumenter.h"
#include "PHINodeInstrumenter.h"
#include "SelectInstrumenter.h"
#include "LandingPadInstrumenter.h"
#include "AllocaInstrumenter.h"
#include "BranchInstrumenter.h"
#include "ReturnInstrumenter.h"
#include "SwitchInstrumenter.h"
#include "IndirectBrInstrumenter.h"
#include "AtomicCmpXchgInstrumenter.h"
#include "FenceInstrumenter.h"
#include "ICmpInstrumenter.h"
#include "FCmpInstrumenter.h"
#include "VAArgInstrumenter.h"
#include "AtomicRMWInstrumenter.h"
#include "ResumeInstrumenter.h"
#include "InvokeInstrumenter.h"

/*******************************************************************************************/

template<class T>
class RegisterInstrumenter {
  public:
    RegisterInstrumenter(std::string name) {
      fprintf(stderr, ">>> Registering instrumenter for instruction: %s\n", name.c_str());

      Instrumentation* I = Instrumentation::GetInstance();
      safe_assert(I != NULL);
      I->RegisterInstrumenter(new T(name, I));
    }
};

/*******************************************************************************************/

// macro for adding instrumenters
#define REGISTER_INSTRUMENTER(T, N) \
  static RegisterInstrumenter<T> T##_INSTANCE(N);

/*******************************************************************************************/

// active instrumenters (see http://llvm.org/docs/LangRef.html)

// ***** Binary Operations ****** //
REGISTER_INSTRUMENTER(AddInstrumenter, "add") //done
REGISTER_INSTRUMENTER(FAddInstrumenter, "fadd") //done
REGISTER_INSTRUMENTER(SubInstrumenter, "sub") // done
REGISTER_INSTRUMENTER(FSubInstrumenter, "fsub") // done 
REGISTER_INSTRUMENTER(MulInstrumenter, "mul") // done
REGISTER_INSTRUMENTER(FMulInstrumenter, "fmul") // done
REGISTER_INSTRUMENTER(UDivInstrumenter, "udiv") // done
REGISTER_INSTRUMENTER(SDivInstrumenter, "sdiv") // done
REGISTER_INSTRUMENTER(FDivInstrumenter, "fdiv") // done
REGISTER_INSTRUMENTER(URemInstrumenter, "urem") // done
REGISTER_INSTRUMENTER(SRemInstrumenter, "srem") // done
REGISTER_INSTRUMENTER(FRemInstrumenter, "frem") // done

// ***** Bitwise Binary Operations ***** //
REGISTER_INSTRUMENTER(ShlInstrumenter, "shl") //done
REGISTER_INSTRUMENTER(LShrInstrumenter, "lshr") //done
REGISTER_INSTRUMENTER(AShrInstrumenter, "ashr") //done
REGISTER_INSTRUMENTER(AndInstrumenter, "and_") //done
REGISTER_INSTRUMENTER(OrInstrumenter, "or_") //done
REGISTER_INSTRUMENTER(XorInstrumenter, "xor_") //done

// ***** Vector Operations ***** //
REGISTER_INSTRUMENTER(ExtractElementInstrumenter, "extractelement") // not in C
REGISTER_INSTRUMENTER(InsertElementInstrumenter, "insertelement") // not in C
REGISTER_INSTRUMENTER(ShuffleVectorInstrumenter, "shufflevector") // not in C

// ***** Aggregate Operations ***** //
REGISTER_INSTRUMENTER(ExtractValueInstrumenter, "extractvalue") // not in C
REGISTER_INSTRUMENTER(InsertValueInstrumenter, "insertvalue") // not in C

// ***** Memory Access and Addressing Operations ***** //
REGISTER_INSTRUMENTER(AllocaInstrumenter, "allocax") //done
REGISTER_INSTRUMENTER(LoadInstrumenter, "load") //done
REGISTER_INSTRUMENTER(StoreInstrumenter, "store") //done
REGISTER_INSTRUMENTER(FenceInstrumenter, "fence") 
REGISTER_INSTRUMENTER(AtomicCmpXchgInstrumenter, "cmpxchg")
REGISTER_INSTRUMENTER(AtomicRMWInstrumenter, "atomicrmw") 
REGISTER_INSTRUMENTER(GetElementPtrInstrumenter, "getelementptr")

// ***** Terminator Instructions ***** //
REGISTER_INSTRUMENTER(BranchInstrumenter, "branch") // done
REGISTER_INSTRUMENTER(IndirectBrInstrumenter, "indirectbr") // done
REGISTER_INSTRUMENTER(InvokeInstrumenter, "invoke")
REGISTER_INSTRUMENTER(ResumeInstrumenter, "resume")
REGISTER_INSTRUMENTER(ReturnInstrumenter, "return_") //done
// REGISTER_INSTRUMENTER(SwitchInstrumenter, "switch_") // done
REGISTER_INSTRUMENTER(UnreachableInstrumenter, "unreachable") // done

// ***** Conversion Operations ***** //
REGISTER_INSTRUMENTER(TruncInstrumenter, "trunc")
REGISTER_INSTRUMENTER(ZExtInstrumenter, "zext")
REGISTER_INSTRUMENTER(SExtInstrumenter, "sext")
REGISTER_INSTRUMENTER(FPTruncInstrumenter, "fptrunc") // done
REGISTER_INSTRUMENTER(FPExtInstrumenter, "fpext") // done 
REGISTER_INSTRUMENTER(FPToUIInstrumenter, "fptoui") // done
REGISTER_INSTRUMENTER(FPToSIInstrumenter, "fptosi") // done
REGISTER_INSTRUMENTER(UIToFPInstrumenter, "uitofp") // done
REGISTER_INSTRUMENTER(SIToFPInstrumenter, "sitofp") // done
REGISTER_INSTRUMENTER(PtrToIntInstrumenter, "ptrtoint")
REGISTER_INSTRUMENTER(IntToPtrInstrumenter, "inttoptr")
REGISTER_INSTRUMENTER(BitCastInstrumenter, "bitcast")

// ***** Other Operations ***** //
REGISTER_INSTRUMENTER(ICmpInstrumenter, "icmp") //done
REGISTER_INSTRUMENTER(FCmpInstrumenter, "fcmp")
REGISTER_INSTRUMENTER(PHINodeInstrumenter, "phinode")
REGISTER_INSTRUMENTER(SelectInstrumenter, "select")
REGISTER_INSTRUMENTER(CallInstrumenter, "call") // cuong: limited support for intrinsic 
REGISTER_INSTRUMENTER(VAArgInstrumenter, "va_arg")
REGISTER_INSTRUMENTER(LandingPadInstrumenter, "landingpad")



/*******************************************************************************************/

char MonitorPass::ID = 0;
static RegisterPass<MonitorPass>
Z("instrument", "MonitorPass");

FunctionPass *llvm::createMonitorPass() {
  return new MonitorPass();
}
