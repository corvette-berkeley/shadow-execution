/**
 * @file InstructionMonitor.cpp
 * @brief
 */

/*
 * Copyright (c) 2013, UC Berkeley All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1.  Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software must
 * display the following acknowledgement: This product includes software
 * developed by the UC Berkeley.
 *
 * 4. Neither the name of the UC Berkeley nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY UC BERKELEY ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL UC BERKELEY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen and Cindy Rubio-Gonzalez

#include "InstructionMonitor.h"
#include "InstructionObserver.h"
#include "PrintObserver.h"
#include "InterpreterObserver.h"
#include "NaNPropagationAnalysis.h"
#include "FPInstabilityAnalysis.h"
// j#include "OutOfBoundAnalysis.h"

/*******************************************************************************************/
#define DISPATCH_TO_OBSERVERS(func, ...) \
	for(ObserverPtrList::iterator itr = observers_.begin(); itr < observers_.end(); ++itr) { \
		(*itr)->func(__VA_ARGS__); \
	}
/*******************************************************************************************/

// ***** Binary Operations ***** //
void llvm_add(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(add, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(fadd, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_sub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(sub, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(fsub, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_mul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(mul, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx){
	DISPATCH_TO_OBSERVERS(fmul, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_udiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(udiv, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_sdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(sdiv, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(fdiv, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_urem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(urem, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_srem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(srem, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_frem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(frem, lScope, rScope, lValue, rValue, type, line, inx)
}

// ***** Bitwise Binary Operations ***** //
void llvm_shl(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(shl, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_lshr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(lshr, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_ashr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(ashr, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_and_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(and_, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_or_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(or_, lScope, rScope, lValue, rValue, type, line, inx)
}

void llvm_xor_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int line, int inx) {
	DISPATCH_TO_OBSERVERS(xor_, lScope, rScope, lValue, rValue, type, line, inx)
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

void llvm_allocax(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, KVALUE* result) {
  DISPATCH_TO_OBSERVERS(allocax, iid, kind, size, inx, line, arg, result)
}

void llvm_allocax_array(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, KVALUE* addr) {
  DISPATCH_TO_OBSERVERS(allocax_array, iid, kind, size, inx, line, arg, addr)
}

void llvm_allocax_struct(IID iid, uint64_t size, int inx, int line, bool arg, KVALUE* addr) {
  DISPATCH_TO_OBSERVERS(allocax_struct, iid, size, inx, line, arg, addr);
}

void llvm_load(IID iid, KIND kind, KVALUE* op, bool loadGlobal, int loadInx, int file, int line, int inx) {
  DISPATCH_TO_OBSERVERS(load, iid, kind, op, loadGlobal, loadInx, file, line, inx);
}

void llvm_load_struct(IID iid, KIND kind, KVALUE* op, int file, int line, int inx) {
  DISPATCH_TO_OBSERVERS(load_struct, iid, kind, op, file, line, inx);
}

void llvm_store(int pInx, SCOPE pScope, KVALUE* op, int file, int line, int inx) {
  DISPATCH_TO_OBSERVERS(store, pInx, pScope, op, file, line, inx)
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

void llvm_getelementptr(IID iid, bool isbound, KVALUE* value, KVALUE* index, KIND kind, uint64_t size, bool loadGlobal, int loadInx, int line, int inx) {
  DISPATCH_TO_OBSERVERS(getelementptr, iid, isbound, value, index, kind, size, loadGlobal, loadInx, line, inx)
}

void llvm_getelementptr_array(KVALUE* value, KIND kind, int elementSize, int scopeInx01, int scopeInx02, int scopeInx03, int64_t valOrInx01, int64_t valOrInx02, int64_t valOrInx03, int size01, int size02,  int inx) {
  DISPATCH_TO_OBSERVERS(getelementptr_array, value, kind, elementSize, scopeInx01, scopeInx02, scopeInx03, valOrInx01, valOrInx02, valOrInx03, size01, size02, inx)
}

void llvm_getelementptr_struct(IID iid, bool isbound, KVALUE* value, KIND kind1, KIND kind2, int inx) {
  DISPATCH_TO_OBSERVERS(getelementptr_struct, iid, isbound, value, kind1, kind2, inx)
}

// ***** Conversion Operations ***** //
void llvm_trunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(trunc, op, opScope, opKind, kind, size, inx);
}

void llvm_zext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(zext, op, opScope, opKind, kind, size, inx);
}

void llvm_sext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(sext, op, opScope, opKind, kind, size, inx);
}

void llvm_fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(fptrunc, op, opScope, opKind, kind, size, inx);
}

void llvm_fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(fpext, op, opScope, opKind, kind, size, inx);
}

void llvm_fptoui(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(fptoui, op, opScope, opKind, kind, size, inx);
}

void llvm_fptosi(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(fptosi, op, opScope, opKind, kind, size, inx);
}

void llvm_uitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(uitofp, op, opScope, opKind, kind, size, inx);
}

void llvm_sitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(sitofp, op, opScope, opKind, kind, size, inx);
}

void llvm_ptrtoint(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(ptrtoint, op, opScope, opKind, kind, size, inx);
}

void llvm_inttoptr(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(inttoptr, op, opScope, opKind, kind, size, inx);
}

void llvm_bitcast(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DISPATCH_TO_OBSERVERS(bitcast, op, opScope, opKind, kind, size, inx);
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

void llvm_push_string(int c) {
  DISPATCH_TO_OBSERVERS(push_string, c);
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

void llvm_push_getelementptr_inx5(int scope01, int scope02, int scope03, int scope04, int scope05, int64_t vori01, int64_t vori02, int64_t vori03, int64_t vori04, int64_t vori05) {
  DISPATCH_TO_OBSERVERS(push_getelementptr_inx5, scope01, scope02, scope03, scope04, scope05, vori01, vori02, vori03, vori04, vori05);
}

void llvm_push_array_size5(int scope01, int scope02, int scope03, int scope04, int scope05) { 
  DISPATCH_TO_OBSERVERS(push_array_size5, scope01, scope02, scope03, scope04, scope05);
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

void llvm_push_struct_element_size(uint64_t s) {
  DISPATCH_TO_OBSERVERS(push_struct_element_size, s);
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

void llvm_after_struct_call() {
  DISPATCH_TO_OBSERVERS(after_struct_call)
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

void llvm_call_malloc(IID iid, bool nounwind, KIND type, KVALUE* value, int size, int inx, KVALUE* mallocAddress) {
  DISPATCH_TO_OBSERVERS(call_malloc, iid, nounwind, type, value, size, inx, mallocAddress)
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
REGISTER_OBSERVER(PrintObserver, "print")
// REGISTER_OBSERVER(InterpreterObserver, "interpreter")
// REGISTER_OBSERVER(FPInstabilityAnalysis, "fpinstability")
// REGISTER_OBSERVER(NaNPropagationAnalysis, "nan")
//REGISTER_OBSERVER(OutOfBoundAnalysis, "outofboundanalysis")

/*******************************************************************************************/


