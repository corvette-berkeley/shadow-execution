#include "InstructionMonitor.h"
#include "InstructionObserver.h"

/*******************************************************************************************/
#define DISPATCH_TO_OBSERVERS(func, ...) \
	for(ObserverPtrList::iterator itr = observers_.begin(); itr < observers_.end(); ++itr) { \
		(*itr)->func(__VA_ARGS__); \
	}
/*******************************************************************************************/

// ***** Binary Operations ***** //
void llvm_add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(add, iid, nuw, nsw, op1, op2)
}

void llvm_fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(fadd, iid, nuw, nsw, op1, op2)
}

void llvm_sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(sub, iid, nuw, nsw, op1, op2)
}

void llvm_fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(fsub, iid, nuw, nsw, op1, op2)
}

void llvm_mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(mul, iid, nuw, nsw, op1, op2)
}

void llvm_fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(fmul, iid, nuw, nsw, op1, op2)
}

void llvm_udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(udiv, iid, nuw, nsw, op1, op2)
}

void llvm_sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(sdiv, iid, nuw, nsw, op1, op2)
}

void llvm_fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
	DISPATCH_TO_OBSERVERS(fdiv, iid, nuw, nsw, op1, op2)
}

void llvm_urem() {
	DISPATCH_TO_OBSERVERS(urem)
}

void llvm_srem() {
	DISPATCH_TO_OBSERVERS(srem)
}

void llvm_frem() {
	DISPATCH_TO_OBSERVERS(frem)
}

// ***** Bitwise Binary Operations ***** //
void llvm_shl() {
	DISPATCH_TO_OBSERVERS(shl)
}

void llvm_lshr() {
	DISPATCH_TO_OBSERVERS(lshr)
}

void llvm_ashr() {
	DISPATCH_TO_OBSERVERS(ashr)
}

void llvm_and_() {
	DISPATCH_TO_OBSERVERS(and_)
}

void llvm_or_() {
	DISPATCH_TO_OBSERVERS(or_)
}

void llvm_xor_() {
	DISPATCH_TO_OBSERVERS(xor_)
}

// ****** Vector Operations ****** //
void llvm_extractelement() {
	DISPATCH_TO_OBSERVERS(extractelement)
}

void llvm_insertelement() {
	DISPATCH_TO_OBSERVERS(insertelement)
}

void llvm_shufflevector() {
	DISPATCH_TO_OBSERVERS(shufflevector)
}

// ***** Aggregate Operations ***** //
void llvm_extractvalue() {
	DISPATCH_TO_OBSERVERS(extractvalue)
}

void llvm_insertvalue() {
	DISPATCH_TO_OBSERVERS(insertvalue)
}


// ***** Memory Access and Addressing Operations ***** //
void llvm_alloca() {
	DISPATCH_TO_OBSERVERS(alloca)
}

void llvm_load() {
	DISPATCH_TO_OBSERVERS(load)
}

/*
void llvm_load(IID iid, PTR addr, KVALUE* value) {
	DISPATCH_TO_OBSERVERS(load, iid, addr, value);
}
*/

void llvm_store(IID iid, PTR addr, KVALUE* value) {
	DISPATCH_TO_OBSERVERS(store, iid, addr, value);
}

void llvm_fence() {
	DISPATCH_TO_OBSERVERS(fence)
}

void llvm_cmpxchg() {
	DISPATCH_TO_OBSERVERS(cmpxchg)
}

void llvm_atomicrmw() {
	DISPATCH_TO_OBSERVERS(atomicrmw)
}

void llvm_getelementptr() {
	DISPATCH_TO_OBSERVERS(getelementptr)
}


// ***** Conversion Operations ***** //
void llvm_trunc() {
	DISPATCH_TO_OBSERVERS(trunc)
}

void llvm_zext() {
	DISPATCH_TO_OBSERVERS(zext)
}

void llvm_sext() {
	DISPATCH_TO_OBSERVERS(sext)
}

void llvm_fptrunc() {
	DISPATCH_TO_OBSERVERS(fptrunc)
}

void llvm_fpext() {
	DISPATCH_TO_OBSERVERS(fpext)
}

void llvm_fptoui() {
	DISPATCH_TO_OBSERVERS(fptoui)
}

void llvm_fptosi() {
	DISPATCH_TO_OBSERVERS(fptosi)
}

void llvm_uitofp() {
	DISPATCH_TO_OBSERVERS(uitofp)
}

void llvm_sitofp() {
	DISPATCH_TO_OBSERVERS(sitofp)
}

void llvm_inttoptr() {
	DISPATCH_TO_OBSERVERS(inttoptr)
}

void llvm_ptrtoint() {
	DISPATCH_TO_OBSERVERS(ptrtoint)
}

void llvm_bitcast() {
	DISPATCH_TO_OBSERVERS(bitcast)
}


// **** Terminator Instructions ***** //
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


// ***** Other Operations ***** //
void llvm_icmp() {
	DISPATCH_TO_OBSERVERS(icmp)
}

void llvm_fcmp() {
	DISPATCH_TO_OBSERVERS(fcmp)
}

void llvm_phinode() {
	DISPATCH_TO_OBSERVERS(phinode)
}

void llvm_select() {
	DISPATCH_TO_OBSERVERS(select)
}

void llvm_call() {
	DISPATCH_TO_OBSERVERS(call)
}

void llvm_vaarg() {
	DISPATCH_TO_OBSERVERS(vaarg)
}

void llvm_landingpad() {
	DISPATCH_TO_OBSERVERS(landingpad)
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


