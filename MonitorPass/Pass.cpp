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

    /********************************************************************************/
    /** cuong:                                                                   ****/
    /** First pass through the function body to compute varCount and indices     ****/
    /** i) varCount: the number of local variables and registers                 ****/ 
    /** ii) indices: a mapping from each local variable and register to an index ****/
    /********************************************************************************/

    // set up varCount and indices map
    instrumentation->BeginFunction();

    // create index for all instructions
    for (Function::iterator BB = F.begin(), e = F.end(); BB != e; ++BB) {
      for (BasicBlock::iterator itr = BB->begin(), end = BB->end(); itr != end; ++itr) {
        Instruction* inst = (Instruction*) itr;
        IID iid = static_cast<IID>(reinterpret_cast<ADDRINT>(inst));
        instrumentation->createIndex(iid);
      }
    }

    /********************************************************************************/
    /** cuong:                                                                   ****/
    /** Second pass through the function body for instrumentation                ****/
    /** i) insert call to llvm_create_stack_frame at the beginning of each       ****/ 
    /**    function                                                              ****/ 
    /** ii) instrument each instruction using instrumentation->CheckAndInstrument ***/
    /********************************************************************************/

    unsigned int skip = 0;
    bool isFirstInstruction = true; 

    for (Function::iterator BB = F.begin(), e = F.end(); BB != e; ++BB) {
      // set up pointers to BB, F, and M
      instrumentation->BeginBasicBlock(BB, &F, M);
      for (BasicBlock::iterator itr = BB->begin(), end = BB->end(); itr != end; ++itr) {

        // insert a call to create stack frame
        if (isFirstInstruction)
        {
          Constant* frameSize = ConstantInt::get(Type::getInt32Ty(instrumentation->M_->getContext()), instrumentation->getFrameSize(), SIGNED);
          TypePtrVector argTypes;
          argTypes.push_back(Type::getInt32Ty(instrumentation->M_->getContext()));

          ValuePtrVector args;
          args.push_back(frameSize);

          FunctionType* funType = FunctionType::get(Type::getVoidTy(instrumentation->M_->getContext()), ArrayRef<Type*>(argTypes), false);
          Instruction* call = CallInst::Create(instrumentation->M_->getOrInsertFunction(StringRef("llvm_create_stack_frame"), funType), ArrayRef<Value*>(args)); 
          call->insertBefore(itr);
          isFirstInstruction = false;
        } 

        //try {

        if (skip == 0) {
          if (instrumentation->CheckAndInstrument(itr)) {
            if (dyn_cast<LoadInst>(itr)) {
              // skip instrumentation of the next 11 instructions in case of a LoadInstr
              skip = 24;
            }
          }
        }
        else {
          skip--;
        }
        //	} catch(...) {
        //return false; // exception!
        //}
      }
    }
    return true;
  }

  bool doInitialization(Module &M) {
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

#include "Instrumenters.h"

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
REGISTER_INSTRUMENTER(ExtractElementInstrumenter, "extractelement")
REGISTER_INSTRUMENTER(InsertElementInstrumenter, "insertelement") 
REGISTER_INSTRUMENTER(ShuffleVectorInstrumenter, "shufflevector") 

// ***** Aggregate Operations ***** //
REGISTER_INSTRUMENTER(ExtractValueInstrumenter, "extractvalue")
REGISTER_INSTRUMENTER(InsertValueInstrumenter, "insertvalue")

// ***** Memory Access and Addressing Operations ***** //
REGISTER_INSTRUMENTER(AllocaInstrumenter, "allocax") //done
REGISTER_INSTRUMENTER(LoadInstrumenter, "load") //done
REGISTER_INSTRUMENTER(StoreInstrumenter, "store") //done
REGISTER_INSTRUMENTER(FenceInstrumenter, "fence") 
REGISTER_INSTRUMENTER(AtomicCmpXchgInstrumenter, "cmpxchg")
REGISTER_INSTRUMENTER(AtomicRMWInstrumenter, "atomicrmw") 
REGISTER_INSTRUMENTER(GetElementPtrInstrumenter, "getelementptr")

// ***** Terminator Instructions ***** //
REGISTER_INSTRUMENTER(BranchInstrumenter, "branch")
REGISTER_INSTRUMENTER(IndirectBrInstrumenter, "indirectbr")
REGISTER_INSTRUMENTER(InvokeInstrumenter, "invoke")
REGISTER_INSTRUMENTER(ResumeInstrumenter, "resume")
REGISTER_INSTRUMENTER(ReturnInstrumenter, "return_") //done
//REGISTER_INSTRUMENTER(SwitchInstrumenter, "switch_")
REGISTER_INSTRUMENTER(UnreachableInstrumenter, "unreachable")

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
//REGISTER_INSTRUMENTER(PHINodeInstrumenter, "phinode")
REGISTER_INSTRUMENTER(SelectInstrumenter, "select")
REGISTER_INSTRUMENTER(CallInstrumenter, "call") // nacuong: not for intrinsic yet
//REGISTER_INSTRUMENTER(VAArgInstrumenter, "va_arg")
//REGISTER_INSTRUMENTER(LandingPadInstrumenter, "landingpad")




/*******************************************************************************************/

char MonitorPass::ID = 0;
static RegisterPass<MonitorPass>
Z("instrument", "MonitorPass");

FunctionPass *llvm::createMonitorPass() {
  return new MonitorPass();
}
