
#include "Common.h"
#include "Instrumentation.h"

#define UNUSED __attribute__((__unused__))

/*******************************************************************************************/

// singleton instance (static)
Instrumentation* Instrumentation::instance_ = NULL;

/*******************************************************************************************/

void Instrumentation::RegisterInstrumenter(InstrumenterBase* instrumenter) {
	instrumenters_.push_back(instrumenter);
}

/*******************************************************************************************/

Instrumentation::~Instrumentation() {
	for(InstrumenterPtrList::iterator itr = instrumenters_.begin();
			itr < instrumenters_.end();
			itr = instrumenters_.erase(itr)) {
		InstrumenterBase* instrumenter = *itr;
		safe_assert(instrumenter != NULL);
		delete instrumenter;
	}
}

/*******************************************************************************************/

bool Instrumentation::Initialize(Module &M UNUSED) {
	return true;
}

/*******************************************************************************************/

bool Instrumentation::Finalize(Module &M UNUSED) {
	for(InstrumenterPtrList::iterator itr = instrumenters_.begin();
			itr < instrumenters_.end();
			itr = instrumenters_.erase(itr)) {
		InstrumenterBase* instrumenter = *itr;
		safe_assert(instrumenter != NULL);
		outs() << instrumenter->ReportToString();
	}
	return true;
}

/*******************************************************************************************/

void Instrumentation::BeginBasicBlock(BasicBlock* BB, Function* F, Module* M) {
	safe_assert(BB != NULL);	BB_ = BB;
	safe_assert(F != NULL);		F_ = F;
	safe_assert(M != NULL);		M_ = M;
	AS_ = 0U;
}

/*******************************************************************************************/

// returns true iff any instrumentation was done
bool Instrumentation::CheckAndInstrument(Instruction* I) {
	// find an instrumenter to handle the given instruction
	for(InstrumenterPtrList::iterator itr = instrumenters_.begin(); itr < instrumenters_.end(); ++itr) {
		InstrumenterBase* instrumenter = *itr;
		safe_assert(instrumenter != NULL);
		safe_assert(instrumenter->parent_ == this);

		if(instrumenter->CheckAndInstrument(I)) {
			return true;
		}
	}
	return false;
}

/*******************************************************************************************/

