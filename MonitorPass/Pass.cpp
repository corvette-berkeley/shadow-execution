#define DEBUG_TYPE "hello"

#include "Instrumentation.h"
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
		for (Function::iterator BB = F.begin(), e = F.end(); BB != e; ++BB) {
			// set up pointers to BB, F, and M
			instrumentation->BeginBasicBlock(BB, &F, M);
			for (BasicBlock::iterator itr = BB->begin(), end = BB->end(); itr != end; ++itr) {
			  //try {
					instrumentation->CheckAndInstrument(itr);
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

// active instrumenters
REGISTER_INSTRUMENTER(LoadInstrumenter, "load")
REGISTER_INSTRUMENTER(StoreInstrumenter, "store")
REGISTER_INSTRUMENTER(AddInstrumenter, "add")
REGISTER_INSTRUMENTER(SubInstrumenter, "sub")
REGISTER_INSTRUMENTER(AllocaInstrumenter, "alloca")
REGISTER_INSTRUMENTER(CallInstrumenter, "call")

// **** CmpInst **** //
REGISTER_INSTRUMENTER(FCmpInstrumenter, "fcmp")
REGISTER_INSTRUMENTER(ICmpInstrumenter, "icmp")

REGISTER_INSTRUMENTER(ExtractElementInstrumenter, "extract_element")
REGISTER_INSTRUMENTER(FenceInstrumenter, "fence")
REGISTER_INSTRUMENTER(GetElementPtrInstrumenter, "get_element_ptr")
REGISTER_INSTRUMENTER(InsertElementInstrumenter, "insert_element")
REGISTER_INSTRUMENTER(InsertValueInstrumenter, "insert_value")
REGISTER_INSTRUMENTER(LandingPadInstrumenter, "landing_pad")
REGISTER_INSTRUMENTER(PHINodeInstrumenter, "phinode")
REGISTER_INSTRUMENTER(SelectInstrumenter, "select")
REGISTER_INSTRUMENTER(ShuffleVectorInstrumenter, "shuffle_vector")

// ***** TerminatorInst ***** //
REGISTER_INSTRUMENTER(BranchInstrumenter, "branch")
REGISTER_INSTRUMENTER(IndirectBrInstrumenter, "indirectbr")
REGISTER_INSTRUMENTER(InvokeInstrumenter, "invoke")
REGISTER_INSTRUMENTER(ResumeInstrumenter, "resume")
REGISTER_INSTRUMENTER(ReturnInstrumenter, "return_")
REGISTER_INSTRUMENTER(SwitchInstrumenter, "switch_")
REGISTER_INSTRUMENTER(UnreachableInstrumenter, "unreachable")

// ***** CastInst ***** //
REGISTER_INSTRUMENTER(BitCastInstrumenter, "bitcast")
REGISTER_INSTRUMENTER(FPExtInstrumenter, "fpext")
REGISTER_INSTRUMENTER(FPToSIInstrumenter, "fptosi")
REGISTER_INSTRUMENTER(FPToUIInstrumenter, "fptoui")
REGISTER_INSTRUMENTER(FPTruncInstrumenter, "fptrunc")




/*******************************************************************************************/

char MonitorPass::ID = 0;
static RegisterPass<MonitorPass>
Z("instrument", "MonitorPass");

FunctionPass *llvm::createMonitorPass() {
  return new MonitorPass();
}
