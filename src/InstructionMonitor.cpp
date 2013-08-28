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

void llvm_fpext() {
	DISPATCH_TO_OBSERVERS(fpext)
}

void llvm_load() {
	DISPATCH_TO_OBSERVERS(load)
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


