#include "InstructionMonitor.h"
#include "InstructionObserver.h"

/*******************************************************************************************/
#define DISPATCH_TO_OBSERVERS(func, ...) \
	for(ObserverPtrList::iterator itr = observers_.begin(); itr < observers_.end(); ++itr) { \
		(*itr)->func(__VA_ARGS__); \
	}
/*******************************************************************************************/
/*
void llvm_load(IID iid, PTR addr, KVALUE* value) {
	DISPATCH_TO_OBSERVERS(load, iid, addr, value);
}
*/
void llvm_store(IID iid, PTR addr, KVALUE* value) {
	DISPATCH_TO_OBSERVERS(store, iid, addr, value);
}

void llvm_add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(add, iid, nuw, nsw, op1, op2)
}

void llvm_sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(sub, iid, nuw, nsw, op1, op2)
}

void llvm_alloca() {
	DISPATCH_TO_OBSERVERS(alloca)
}

void llvm_call() {
	DISPATCH_TO_OBSERVERS(call)
}

// ***** CastInst ***** //

void llvm_bitcast() {
	DISPATCH_TO_OBSERVERS(bitcast)
}

void llvm_fpext() {
	DISPATCH_TO_OBSERVERS(fpext)
}

void llvm_fptosi() {
	DISPATCH_TO_OBSERVERS(fptosi)
}

void llvm_fptoui() {
	DISPATCH_TO_OBSERVERS(fptoui)
}

void llvm_fptrunc() {
	DISPATCH_TO_OBSERVERS(fptrunc)
}


void llvm_load() {
	DISPATCH_TO_OBSERVERS(load)
}

// **** TerminatorInst ***** //

void llvm_branch() {
	DISPATCH_TO_OBSERVERS(branch)
}

void llvm_indirectbr() {
	DISPATCH_TO_OBSERVERS(indirectbr)
}

void llvm_invoke() {
	DISPATCH_TO_OBSERVERS(invoke)
}

void llvm_resume() {
	DISPATCH_TO_OBSERVERS(resume)
}

void llvm_return_() {
	DISPATCH_TO_OBSERVERS(return_)
}

void llvm_switch_() {
	DISPATCH_TO_OBSERVERS(switch_)
}

void llvm_unreachable() {
	DISPATCH_TO_OBSERVERS(unreachable)
}



/*******************************************************************************************/

ObserverPtrList observers_;

/*******************************************************************************************/

#include "Observers.h"

// macro for adding observers
#define REGISTER_OBSERVER(T, N) \
		static RegisterObserver<T> T##_INSTANCE(N);

// active observers
REGISTER_OBSERVER(PrintObserver, "print")

/*******************************************************************************************/


