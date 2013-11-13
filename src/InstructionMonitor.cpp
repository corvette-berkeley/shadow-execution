#include "InstructionMonitor.h"
#include "InstructionObserver.h"
#include "InterpreterObserver.h"
#include "OutOfBoundAnalysis.h"
#include "PrintObserver.h"

/*******************************************************************************************/
#define DISPATCH_TO_OBSERVERS(func, ...) \
	for(ObserverPtrList::iterator itr = observers_.begin(); itr < observers_.end(); ++itr) { \
		(*itr)->func(__VA_ARGS__); \
	}
/*******************************************************************************************/

// ***** Binary Operations ***** //
void llvm_add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(add, iid, nuw, nsw, op1, op2, inx)
}

void llvm_fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(fadd, iid, nuw, nsw, op1, op2, inx)
}

void llvm_sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(sub, iid, nuw, nsw, op1, op2, inx)
}

void llvm_fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(fsub, iid, nuw, nsw, op1, op2, inx)
}

void llvm_mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(mul, iid, nuw, nsw, op1, op2, inx)
}

void llvm_fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx){
	DISPATCH_TO_OBSERVERS(fmul, iid, nuw, nsw, op1, op2, inx)
}

void llvm_udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(udiv, iid, nuw, nsw, op1, op2, inx)
}

void llvm_sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(sdiv, iid, nuw, nsw, op1, op2, inx)
}

void llvm_fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(fdiv, iid, nuw, nsw, op1, op2, inx)
}

void llvm_urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(urem, iid, nuw, nsw, op1, op2, inx)
}

void llvm_srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(srem, iid, nuw, nsw, op1, op2, inx)
}

void llvm_frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(frem, iid, nuw, nsw, op1, op2, inx)
}

// ***** Bitwise Binary Operations ***** //
void llvm_shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(shl, iid, nuw, nsw, op1, op2, inx)
}

void llvm_lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(lshr, iid, nuw, nsw, op1, op2, inx)
}

void llvm_ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(ashr, iid, nuw, nsw, op1, op2, inx)
}

void llvm_and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(and_, iid, nuw, nsw, op1, op2, inx)
}

void llvm_or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(or_, iid, nuw, nsw, op1, op2, inx)
}

void llvm_xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(xor_, iid, nuw, nsw, op1, op2, inx)
}

// ****** Vector Operations ****** //
void llvm_extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(extractelement, iid, op1, op2, inx)
}

void llvm_insertelement() {
	DISPATCH_TO_OBSERVERS(insertelement)
}

void llvm_shufflevector() {
	DISPATCH_TO_OBSERVERS(shufflevector)
}

// ***** Aggregate Operations ***** //
void llvm_extractvalue(IID iid, int inx, int opinx) {
	DISPATCH_TO_OBSERVERS(extractvalue, iid, inx, opinx)
}

void llvm_insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(insertvalue, iid, op1, op2, inx)
}


// ***** Memory Access and Addressing Operations ***** //

void llvm_allocax(IID iid, KIND kind, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(allocax, iid, kind, size, inx)
}

void llvm_allocax_array(IID iid, KIND kind, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(allocax_array, iid, kind, size, inx)
}

void llvm_allocax_struct(IID iid, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(allocax_struct, iid, size, inx);
}

void llvm_load(IID iid, KIND kind, KVALUE* op, int line, int inx) {
  DISPATCH_TO_OBSERVERS(load, iid, kind, op, line, inx);
}


void llvm_store(IID iid, KVALUE* op, KVALUE* value, int line, int inx) {
  DISPATCH_TO_OBSERVERS(store, iid, op, value, line, inx)
}

void llvm_fence() {
	DISPATCH_TO_OBSERVERS(fence)
}

void llvm_cmpxchg(IID iid, PTR addr, KVALUE* value1, KVALUE* value2, int inx) {
  DISPATCH_TO_OBSERVERS(cmpxchg, iid, addr, value1, value2, inx)
}

void llvm_atomicrmw() {
	DISPATCH_TO_OBSERVERS(atomicrmw)
}

void llvm_getelementptr(IID iid, bool isbound, KVALUE* value, KVALUE* index, KIND kind, uint64_t size, int line, int inx) {
  DISPATCH_TO_OBSERVERS(getelementptr, iid, isbound, value, index, kind, size, line, inx)
}

void llvm_getelementptr_array(IID iid, bool isbound, KVALUE* value, KIND kind, int inx) {
  DISPATCH_TO_OBSERVERS(getelementptr_array, iid, isbound, value, kind, inx)
}


void llvm_getelementptr_struct(IID iid, bool isbound, KVALUE* value, KIND kind1, KIND kind2, int inx) {
  DISPATCH_TO_OBSERVERS(getelementptr_struct, iid, isbound, value, kind1, kind2, inx)
}

// ***** Conversion Operations ***** //
void llvm_trunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(trunc, iid, type, op, size, inx);
}

void llvm_zext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(zext, iid, type, op, size, inx);
}

void llvm_sext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(sext, iid, type, op, size, inx);
}

void llvm_fptrunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(fptrunc, iid, type, op, size, inx);
}

void llvm_fpext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(fpext, iid, type, op, size, inx);
}

void llvm_fptoui(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(fptoui, iid, type, op, size, inx);
}

void llvm_fptosi(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(fptosi, iid, type, op, size, inx);
}

void llvm_uitofp(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(uitofp, iid, type, op, size, inx);
}

void llvm_sitofp(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(sitofp, iid, type, op, size, inx);
}

void llvm_ptrtoint(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(ptrtoint, iid, type, op, size, inx);
}

void llvm_inttoptr(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(inttoptr, iid, type, op, size, inx);
}

void llvm_bitcast(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DISPATCH_TO_OBSERVERS(bitcast, iid, type, op, size, inx);
}


// **** Terminator Instructions ***** //
void llvm_branch(IID iid, bool conditional, KVALUE* op1, int inx) {
	DISPATCH_TO_OBSERVERS(branch, iid, conditional, op1, inx)
}

void llvm_branch2(IID iid, bool conditional, int inx) {
	DISPATCH_TO_OBSERVERS(branch2, iid, conditional, inx)
}

void llvm_indirectbr(IID iid, KVALUE* op1, int inx) {
	DISPATCH_TO_OBSERVERS(indirectbr, iid, op1, inx)
}

void llvm_invoke(IID iid, KVALUE* op, int inx) {
	DISPATCH_TO_OBSERVERS(invoke, iid, op, inx)
}

void llvm_resume(IID iid, KVALUE* op1, int inx) {
	DISPATCH_TO_OBSERVERS(resume, iid, op1, inx)
}

void llvm_return_(IID iid, KVALUE* op1, int inx) {
	DISPATCH_TO_OBSERVERS(return_, iid, op1, inx)
}

void llvm_return_struct_(IID iid, int inx, int valInx) {
  DISPATCH_TO_OBSERVERS(return_struct_, iid, inx, valInx);
}

void llvm_return2_(IID iid, int inx) {
	DISPATCH_TO_OBSERVERS(return2_, iid, inx)
}

void llvm_switch_(IID iid, KVALUE* op, int inx) {
	DISPATCH_TO_OBSERVERS(switch_, iid, op, inx)
}

void llvm_unreachable() {
	DISPATCH_TO_OBSERVERS(unreachable)
}


// ***** Other Operations ***** //
void llvm_icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  DISPATCH_TO_OBSERVERS(icmp, iid, op1, op2, pred, inx)
}

void llvm_fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  DISPATCH_TO_OBSERVERS(fcmp, iid, op1, op2, pred, inx)
}

void llvm_phinode(IID iid, int inx) {
	DISPATCH_TO_OBSERVERS(phinode, iid, inx)
}

void llvm_select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {
  DISPATCH_TO_OBSERVERS(select, iid, cond, tvalue, fvalue, inx)
}

void llvm_push_stack(KVALUE* value) {
	DISPATCH_TO_OBSERVERS(push_stack, value)
}

void llvm_push_phinode_constant_value(KVALUE* value, int blockId) {
  DISPATCH_TO_OBSERVERS(push_phinode_constant_value, value, blockId);
}

void llvm_push_phinode_value(int valId, int blockId) {
  DISPATCH_TO_OBSERVERS(push_phinode_value, valId, blockId);
}

void llvm_push_return_struct(KVALUE* value) {
	DISPATCH_TO_OBSERVERS(push_return_struct, value)
}

void llvm_push_getelementptr_inx(KVALUE* value) {
  DISPATCH_TO_OBSERVERS(push_getelementptr_inx, value)
}

void llvm_push_getelementptr_inx2(int value) {
  DISPATCH_TO_OBSERVERS(push_getelementptr_inx2, value)
}

void llvm_push_array_size(uint64_t i) {
  DISPATCH_TO_OBSERVERS(push_array_size, i);
}

void llvm_push_struct_type(KIND kind) {
  DISPATCH_TO_OBSERVERS(push_struct_type, kind);
} 

void llvm_construct_array_type(uint64_t i) {
  DISPATCH_TO_OBSERVERS(construct_array_type, i);
}

void llvm_after_call(KVALUE* value) {
  DISPATCH_TO_OBSERVERS(after_call, value)
}

void llvm_after_void_call() {
  DISPATCH_TO_OBSERVERS(after_void_call)
}

void llvm_create_stack_frame(int size) {
  DISPATCH_TO_OBSERVERS(create_stack_frame, size)
}

void llvm_create_global_symbol_table(int size) {
  DISPATCH_TO_OBSERVERS(create_global_symbol_table, size)
}

void llvm_record_block_id(int id) {
  DISPATCH_TO_OBSERVERS(record_block_id, id)
}

void llvm_create_global(KVALUE* value, KVALUE* initializer) {
  DISPATCH_TO_OBSERVERS(create_global, value, initializer)
}

void llvm_call(IID iid, bool nounwind, KIND type, int inx) {
  DISPATCH_TO_OBSERVERS(call, iid, nounwind, type, inx)
}

void llvm_call_malloc(IID iid, bool nounwind, KIND type, KVALUE* value, int size, int inx) {
  DISPATCH_TO_OBSERVERS(call_malloc, iid, nounwind, type, value, size, inx)
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


// macro for adding observers
#define REGISTER_OBSERVER(T, N) \
		static RegisterObserver<T> T##_INSTANCE(N);

// active observers
//REGISTER_OBSERVER(PrintObserver, "print")
REGISTER_OBSERVER(InterpreterObserver, "interpreter")
//REGISTER_OBSERVER(OutOfBoundAnalysis, "outofboundanalysis")

/*******************************************************************************************/


