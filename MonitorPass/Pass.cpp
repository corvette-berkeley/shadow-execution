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

// active instrumenters (see http://llvm.org/docs/LangRef.html)

// ***** Binary Operations ****** //
REGISTER_INSTRUMENTER(AddInstrumenter, "add")
REGISTER_INSTRUMENTER(FAddInstrumenter, "fadd")
REGISTER_INSTRUMENTER(SubInstrumenter, "sub")
REGISTER_INSTRUMENTER(FSubInstrumenter, "fsub")
REGISTER_INSTRUMENTER(MulInstrumenter, "mul")
REGISTER_INSTRUMENTER(FMulInstrumenter, "fmul")
REGISTER_INSTRUMENTER(UDivInstrumenter, "udiv")
REGISTER_INSTRUMENTER(SDivInstrumenter, "sdiv")
REGISTER_INSTRUMENTER(FDivInstrumenter, "fdiv")
REGISTER_INSTRUMENTER(URemInstrumenter, "urem")
REGISTER_INSTRUMENTER(SRemInstrumenter, "srem")
REGISTER_INSTRUMENTER(FRemInstrumenter, "frem")

// ***** Bitwise Binary Operations ***** //
REGISTER_INSTRUMENTER(ShlInstrumenter, "shl")
REGISTER_INSTRUMENTER(LShrInstrumenter, "lshr")
REGISTER_INSTRUMENTER(AShrInstrumenter, "ashr")
REGISTER_INSTRUMENTER(AndInstrumenter, "and_")
REGISTER_INSTRUMENTER(OrInstrumenter, "or_")
REGISTER_INSTRUMENTER(XorInstrumenter, "xor_")

// ***** Vector Operations ***** //
REGISTER_INSTRUMENTER(ExtractElementInstrumenter, "extractelement")
REGISTER_INSTRUMENTER(InsertElementInstrumenter, "insertelement") // printout for now
REGISTER_INSTRUMENTER(ShuffleVectorInstrumenter, "shufflevector") // printout for now

// ***** Aggregate Operations ***** //
REGISTER_INSTRUMENTER(ExtractValueInstrumenter, "extractvalue")
REGISTER_INSTRUMENTER(InsertValueInstrumenter, "insertvalue")

// ***** Memory Access and Addressing Operations ***** //
REGISTER_INSTRUMENTER(AllocaInstrumenter, "allocax")
// REGISTER_INSTRUMENTER(LoadInstrumenter, "load")
REGISTER_INSTRUMENTER(StoreInstrumenter, "store")
REGISTER_INSTRUMENTER(FenceInstrumenter, "fence") // printout for now
REGISTER_INSTRUMENTER(AtomicCmpXchgInstrumenter, "cmpxchg")
REGISTER_INSTRUMENTER(AtomicRMWInstrumenter, "atomicrmw") // printout for now
REGISTER_INSTRUMENTER(GetElementPtrInstrumenter, "getelementptr")

// ***** Terminator Instructions ***** //
REGISTER_INSTRUMENTER(BranchInstrumenter, "branch")
REGISTER_INSTRUMENTER(IndirectBrInstrumenter, "indirectbr")
REGISTER_INSTRUMENTER(InvokeInstrumenter, "invoke")
REGISTER_INSTRUMENTER(ResumeInstrumenter, "resume")
REGISTER_INSTRUMENTER(ReturnInstrumenter, "return_")
//REGISTER_INSTRUMENTER(SwitchInstrumenter, "switch_")
REGISTER_INSTRUMENTER(UnreachableInstrumenter, "unreachable")

// ***** Conversion Operations ***** //
REGISTER_INSTRUMENTER(TruncInstrumenter, "trunc")
REGISTER_INSTRUMENTER(ZExtInstrumenter, "zext")
REGISTER_INSTRUMENTER(SExtInstrumenter, "sext")
REGISTER_INSTRUMENTER(FPTruncInstrumenter, "fptrunc")
REGISTER_INSTRUMENTER(FPExtInstrumenter, "fpext")
REGISTER_INSTRUMENTER(FPToUIInstrumenter, "fptoui")
REGISTER_INSTRUMENTER(FPToSIInstrumenter, "fptosi")
REGISTER_INSTRUMENTER(UIToFPInstrumenter, "uitofp")
REGISTER_INSTRUMENTER(SIToFPInstrumenter, "sitofp")
REGISTER_INSTRUMENTER(PtrToIntInstrumenter, "ptrtoint")
REGISTER_INSTRUMENTER(IntToPtrInstrumenter, "inttoptr")
REGISTER_INSTRUMENTER(BitCastInstrumenter, "bitcast")

// ***** Other Operations ***** //
REGISTER_INSTRUMENTER(ICmpInstrumenter, "icmp")
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
