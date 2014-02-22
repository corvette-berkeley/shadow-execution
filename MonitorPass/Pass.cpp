#define DEBUG_TYPE "hello"

#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include "Instrumentation.h"
#include "Instrumenter.h"
#include "MonitorPass.h"

#include <fstream>
#include <sstream>
#include <set>

cl::opt<string> FileName("file", cl::value_desc("filename"), cl::desc("File names and indexes"), cl::init("metadata.txt"));

cl::opt<string> IncludedFunctions("includedFunctions", cl::value_desc("filename"), cl::desc("List of functions to be instrumented; if skipped all functions are instrumented"), cl::init("included.txt"));

cl::opt<string> LogFileName("logfile", cl::value_desc("filename"), cl::desc("Name of log file"), cl::init("log"));

namespace {

  struct MonitorPass : public FunctionPass {
    static char ID;

    set<string> includedFunctions;


    MonitorPass() : FunctionPass(ID) {}
    ~MonitorPass() {}

    virtual bool runOnFunction(Function &F) {

      if (!includedFunctions.empty()) {
        string name = F.getName().str();

        if (includedFunctions.find(name) == includedFunctions.end()) {
          return true;
        }
      }

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

      // create index for each arguments
      for (Function::arg_iterator ARG = F.arg_begin(), ae = F.arg_end(); ARG != ae; ++ARG) {
        Argument* arg = (Argument*) ARG;
        IID iid = static_cast<IID>(reinterpret_cast<ADDRINT>(arg));
        instrumentation->createIndex(iid);
      }

      // create index for all instructions
      for (Function::iterator BB = F.begin(), e = F.end(); BB != e; ++BB) {
        // creater index for each basic block
        BasicBlock* block = (BasicBlock*) BB;
        IID blockIID = static_cast<IID>(reinterpret_cast<ADDRINT>(block));
        instrumentation->createBlockIndex(blockIID);

        // create index for each instructions
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

      // unsigned int skip = 0;
      bool isFirstInstruction = true; 
      bool isFirstBlockInstruction = true;

      Function::iterator BB = F.begin();
      if (F.getName() == "main") {
        // completely skip first block
        BB++;
      }

      for (Function::iterator e = F.end(); BB != e; ++BB) {

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
              CallInst::Create(instrumentation->M_->getOrInsertFunction(StringRef("llvm_record_block_id"), funType),
                  ArrayRef<Value*>(args)); 

            if (dyn_cast<PHINode>(itr)) {
              // find the last phiNode in the block
              BasicBlock::iterator itr_tmp = itr;
              while(dyn_cast<PHINode>(itr_tmp)) {
                itr_tmp++;
              }
              itr_tmp--; 
              call->insertAfter(itr_tmp);
            } else {
              call->insertBefore(itr);
            }

            isFirstBlockInstruction = false;	   
          }

          // instrumentation
          if (instrumentation->getIndex(itr) != -1) {
            instrumentation->CheckAndInstrument(itr);
          }

        }
      }
      return true;
    }

    bool doInitialization(Module &M) {

      ifstream inFile(IncludedFunctions.c_str());
      string name;

      if (inFile) {
        while (inFile >> name) {
          includedFunctions.insert(name);
        }

        inFile.close();
      }

      Instrumentation* instrumentation = Instrumentation::GetInstance();
      instrumentation->BeginModule(&M);
      Instrumenter* instrumenter = new Instrumenter("", instrumentation);

      // split first block
      Function* mainFunction = M.getFunction("main");
      BasicBlock* firstBlock = &mainFunction->getEntryBlock();
      firstBlock->splitBasicBlock(firstBlock->begin(), "");

      // creating global indices
      for(Module::global_iterator i = M.global_begin(), e = M.global_end(); i != e; i++) {    
        //if (GlobalValue::isPrivateLinkage(i->getLinkage())) {
        IID iid = static_cast<IID>(reinterpret_cast<ADDRINT>(&*i));
        instrumentation->createGlobalIndex(iid);
        // }
      }

      // call back to pass log file name
      InstrPtrVector instrsLog;
      int length;

      length = LogFileName.length();

      for (int i = 0; i < length; i++) {
        int c; 
        Constant *cC;
        ValuePtrVector args;
        TypePtrVector argTypes;
        Instruction* call;
        
        c = LogFileName[i] - '0';
        cC = instrumenter->INT32_CONSTANT(c, SIGNED);
        args.push_back(cC);
        argTypes.push_back(instrumenter->INT32_TYPE());
        call = instrumenter->CALL_INSTR("llvm_push_string",
            instrumenter->VOID_FUNC_TYPE(argTypes), args);
        instrsLog.push_back(call);
      }

      instrumenter->InsertAllBefore(instrsLog, firstBlock->getTerminator());

      // call back to create global symbol table
      InstrPtrVector instrs;
      ValuePtrVector args;
      Constant* size = instrumenter->INT32_CONSTANT(instrumentation->getNumGlobalVar(), NOSIGN);
      args.push_back(size);

      TypePtrVector argTypes;
      argTypes.push_back(instrumenter->INT32_TYPE());

      Instruction* call = instrumenter->CALL_INSTR("llvm_create_global_symbol_table", instrumenter->VOID_FUNC_TYPE(argTypes), args);
      instrs.push_back(call);

      instrumenter->InsertAllBefore(instrs, firstBlock->getTerminator());

      // call back to create each global
      for(Module::global_iterator i = M.global_begin(), e = M.global_end(); i != e; i++) {    
        //if (GlobalValue::isPrivateLinkage(i->getLinkage())) {
        i->dump();

        InstrPtrVector instrs;
        ValuePtrVector args;
        Value* global = instrumenter->KVALUE_VALUE(i, instrs, NOSIGN);
        args.push_back(global);

        if (i->hasInitializer()) {
          KIND initKind = instrumenter->TypeToKind(i->getInitializer()->getType());
          if (initKind != ARRAY_KIND && initKind != STRUCT_KIND) {
            Value* initializer = instrumenter->KVALUE_VALUE(i->getInitializer(), instrs, NOSIGN);
            args.push_back(initializer);
          }
          else {
            // for now
            args.push_back(global);
          }
        }
        else {
          // for now
          args.push_back(global);
        }

        TypePtrVector argTypes;
        argTypes.push_back(instrumenter->KVALUEPTR_TYPE());
        argTypes.push_back(instrumenter->KVALUEPTR_TYPE());	

        Instruction* call = instrumenter->CALL_INSTR("llvm_create_global", instrumenter->VOID_FUNC_TYPE(argTypes), args);
        instrs.push_back(call);

        instrumenter->InsertAllBefore(instrs, firstBlock->getTerminator());
        //} // isPrivateLinkage
      } // global iterator

      return Instrumentation::GetInstance()->Initialize(M);
    }

    bool doFinalization(Module &M) {
      // printing filenames
      Instrumentation *instrumentation = Instrumentation::GetInstance();
      instrumentation->PrintFiles(FileName);

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
REGISTER_INSTRUMENTER(CallInstrumenter, "call") 
REGISTER_INSTRUMENTER(VAArgInstrumenter, "va_arg")
REGISTER_INSTRUMENTER(LandingPadInstrumenter, "landingpad")


/*******************************************************************************************/

char MonitorPass::ID = 0;
static RegisterPass<MonitorPass>
Z("instrument", "MonitorPass");

FunctionPass *llvm::createMonitorPass() {
  return new MonitorPass();
}
