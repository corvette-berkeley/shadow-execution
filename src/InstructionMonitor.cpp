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
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen and Cindy Rubio-Gonzalez

#include "InstructionMonitor.h"
#include "InstructionObserver.h"
#include "InterpreterObserver.h"
#include "EmptyObserver.h"
#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

/*******************************************************************************************/
vector<unique_ptr<InstructionObserver>> observers_ = {};

#define DISPATCH_TO_OBSERVERS_NOARG(func)                                      \
  for (auto &ob_ptr : observers_) {                                            \
    ob_ptr->func();                                                            \
  }

#define DISPATCH_TO_OBSERVERS(func, ...)                                       \
  for (auto &ob_ptr : observers_) {                                            \
    ob_ptr->func(__VA_ARGS__);                                                 \
  }

/*******************************************************************************************/

// macro for adding observers
#define REGISTER_OBSERVER(T, N) static RegisterObserver<T> T##_INSTANCE(N);

REGISTER_OBSERVER(InterpreterObserver, "interpreter")
// REGISTER_OBSERVER(EmptyObserver, "emptyobserver")

/*******************************************************************************************/

// ***** Binary Operations ***** //
void llvm_add(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			  int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(add, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_fadd(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(fadd, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_sub(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			  int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(sub, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_fsub(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(fsub, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_mul(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			  int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(mul, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_fmul(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(fmul, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_udiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(udiv, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_sdiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(sdiv, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_fdiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(fdiv, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_urem(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(urem, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_srem(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(srem, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

void llvm_frem(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
			   int64_t lValue, int64_t rValue, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(frem, iid, liid, riid, lScope, rScope, lValue, rValue,
						  type, inx)
}

// ***** Bitwise Binary Operations ***** //
void llvm_shl(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			  KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(shl, lScope, rScope, lValue, rValue, type, inx)
}

void llvm_lshr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			   KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(lshr, lScope, rScope, lValue, rValue, type, inx)
}

void llvm_ashr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			   KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(ashr, lScope, rScope, lValue, rValue, type, inx)
}

void llvm_and_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			   KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(and_, lScope, rScope, lValue, rValue, type, inx)
}

void llvm_or_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			  KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(or_, lScope, rScope, lValue, rValue, type, inx)
}

void llvm_xor_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			   KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(xor_, lScope, rScope, lValue, rValue, type, inx)
}

// ****** Vector Operations ****** //
void llvm_extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(extractelement, iid, op1, op2, inx)
}

void llvm_insertelement() {
	DISPATCH_TO_OBSERVERS_NOARG(insertelement)
}

void llvm_shufflevector() {
	DISPATCH_TO_OBSERVERS_NOARG(shufflevector)
}

// ***** Aggregate Operations ***** //
void llvm_extractvalue(IID iid, int inx, int opinx) {
	DISPATCH_TO_OBSERVERS(extractvalue, iid, inx, opinx)
}

void llvm_insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
	DISPATCH_TO_OBSERVERS(insertvalue, iid, op1, op2, inx)
}

// ***** Memory Access and Addressing Operations ***** //

void llvm_allocax(IID iid, KIND kind, uint64_t size, int inx, uint64_t addr) {
	DISPATCH_TO_OBSERVERS(allocax, iid, kind, size, inx, addr)
}

void llvm_allocax_array(IID iid, KIND kind, uint64_t size, int inx,
						uint64_t addr) {
	DISPATCH_TO_OBSERVERS(allocax_array, iid, kind, size, inx, addr)
}

void llvm_allocax_struct(IID iid, uint64_t size, int inx, uint64_t addr) {
	DISPATCH_TO_OBSERVERS(allocax_struct, iid, size, inx, addr);
}

void llvm_load(IID iid, KIND kind, SCOPE opScope, int opInx, uint64_t opAddr,
			   bool loadGlobal, int loadInx, int inx) {
	DISPATCH_TO_OBSERVERS(load, iid, kind, opScope, opInx, opAddr, loadGlobal,
						  loadInx, inx);
}

void llvm_load_struct(IID iid, KIND kind, KVALUE* op, int inx) {
	DISPATCH_TO_OBSERVERS(load_struct, iid, kind, op, inx);
}

void llvm_store(int pInx, SCOPE pScope, KIND srcKind, SCOPE srcScope,
				int srcInx, int64_t srcValue) {
	DISPATCH_TO_OBSERVERS(store, pInx, pScope, srcKind, srcScope, srcInx,
						  srcValue)
}

void llvm_fence() {
	DISPATCH_TO_OBSERVERS_NOARG(fence)
}

void llvm_cmpxchg(IID iid, PTR addr, KVALUE* value1, KVALUE* value2, int inx) {
	DISPATCH_TO_OBSERVERS(cmpxchg, iid, addr, value1, value2, inx)
}

void llvm_atomicrmw() {
	DISPATCH_TO_OBSERVERS_NOARG(atomicrmw)
}

void llvm_getelementptr(IID iid, int baseInx, SCOPE baseScope,
						uint64_t baseAddr, int offsetInx, int64_t offsetValue,
						KIND kind, uint64_t size, bool loadGlobal, int loadInx,
						int inx) {
	DISPATCH_TO_OBSERVERS(getelementptr, iid, baseInx, baseScope, baseAddr,
						  offsetInx, offsetValue, kind, size, loadGlobal, loadInx,
						  inx)
}

void llvm_getelementptr_array(int baseInx, SCOPE baseScope, uint64_t baseAddr,
							  int elementSize, int scopeInx01, int scopeInx02,
							  int scopeInx03, int64_t valOrInx01,
							  int64_t valOrInx02, int64_t valOrInx03,
							  int size01, int size02, int inx) {
	DISPATCH_TO_OBSERVERS(getelementptr_array, baseInx, baseScope, baseAddr,
						  elementSize, scopeInx01, scopeInx02, scopeInx03,
						  valOrInx01, valOrInx02, valOrInx03, size01, size02, inx)
}

void llvm_getelementptr_struct(IID iid, int baseInx, SCOPE baseScope,
							   uint64_t baseAddr, int inx) {
	DISPATCH_TO_OBSERVERS(getelementptr_struct, iid, baseInx, baseScope, baseAddr,
						  inx)
}

// ***** Conversion Operations ***** //
void llvm_trunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				int inx) {
	DISPATCH_TO_OBSERVERS(trunc, op, opScope, opKind, kind, size, inx);
}

void llvm_zext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
			   int inx) {
	DISPATCH_TO_OBSERVERS(zext, op, opScope, opKind, kind, size, inx);
}

void llvm_sext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
			   int inx) {
	DISPATCH_TO_OBSERVERS(sext, op, opScope, opKind, kind, size, inx);
}

void llvm_fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				  int inx) {
	DISPATCH_TO_OBSERVERS(fptrunc, op, opScope, opKind, kind, size, inx);
}

void llvm_fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				int inx) {
	DISPATCH_TO_OBSERVERS(fpext, op, opScope, opKind, kind, size, inx);
}

void llvm_fptoui(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				 int inx) {
	DISPATCH_TO_OBSERVERS(fptoui, op, opScope, opKind, kind, size, inx);
}

void llvm_fptosi(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				 int inx) {
	DISPATCH_TO_OBSERVERS(fptosi, op, opScope, opKind, kind, size, inx);
}

void llvm_uitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				 int inx) {
	DISPATCH_TO_OBSERVERS(uitofp, op, opScope, opKind, kind, size, inx);
}

void llvm_sitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				 int inx) {
	DISPATCH_TO_OBSERVERS(sitofp, op, opScope, opKind, kind, size, inx);
}

void llvm_ptrtoint(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				   int inx) {
	DISPATCH_TO_OBSERVERS(ptrtoint, op, opScope, opKind, kind, size, inx);
}

void llvm_inttoptr(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				   int inx) {
	DISPATCH_TO_OBSERVERS(inttoptr, op, opScope, opKind, kind, size, inx);
}

void llvm_bitcast(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				  int inx) {
	DISPATCH_TO_OBSERVERS(bitcast, op, opScope, opKind, kind, size, inx);
}

// **** Terminator Instructions ***** //
void llvm_branch(IID iid, bool conditional, int valInx, SCOPE scope, KIND type,
				 uint64_t value) {
	DISPATCH_TO_OBSERVERS(branch, iid, conditional, valInx, scope, type, value)
}

void llvm_branch2(IID iid, bool conditional) {
	DISPATCH_TO_OBSERVERS(branch2, iid, conditional)
}

void llvm_indirectbr(IID iid, KVALUE* op1, int inx) {
	DISPATCH_TO_OBSERVERS(indirectbr, iid, op1, inx)
}

/*
void llvm_invoke(IID iid, KVALUE* op, int inx) {
	DISPATCH_TO_OBSERVERS(invoke, iid, op, inx)
}
*/

void llvm_resume(IID iid, KVALUE* op1, int inx) {
	DISPATCH_TO_OBSERVERS(resume, iid, op1, inx)
}

void llvm_return_(IID iid, int valInx, SCOPE scope, KIND type, int64_t value) {
	DISPATCH_TO_OBSERVERS(return_, iid, valInx, scope, type, value)
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
	DISPATCH_TO_OBSERVERS_NOARG(unreachable)
}

// ***** Other Operations ***** //
void llvm_icmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			   KIND type, PRED pred, int inx) {
	DISPATCH_TO_OBSERVERS(icmp, lScope, rScope, lValue, rValue, type, pred, inx);
}

void llvm_fcmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
			   KIND type, PRED pred, int inx) {
	DISPATCH_TO_OBSERVERS(fcmp, lScope, rScope, lValue, rValue, type, pred, inx);
}

void llvm_phinode(IID iid, int inx) {
	DISPATCH_TO_OBSERVERS(phinode, iid, inx)
}

void llvm_select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue,
				 int inx) {
	DISPATCH_TO_OBSERVERS(select, iid, cond, tvalue, fvalue, inx)
}

void llvm_push_string(int c) {
	DISPATCH_TO_OBSERVERS(push_string, c);
}

void llvm_push_stack(int inx, SCOPE scope, KIND type, uint64_t addr) {
	DISPATCH_TO_OBSERVERS(push_stack, inx, scope, type, addr)
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

void llvm_push_getelementptr_inx(uint64_t value) {
	DISPATCH_TO_OBSERVERS(push_getelementptr_inx, value)
}

void llvm_push_getelementptr_inx5(int scope01, int scope02, int scope03,
								  int scope04, int scope05, int64_t vori01,
								  int64_t vori02, int64_t vori03,
								  int64_t vori04, int64_t vori05) {
	DISPATCH_TO_OBSERVERS(push_getelementptr_inx5, scope01, scope02, scope03,
						  scope04, scope05, vori01, vori02, vori03, vori04,
						  vori05);
}

void llvm_push_array_size5(int scope01, int scope02, int scope03, int scope04,
						   int scope05) {
	DISPATCH_TO_OBSERVERS(push_array_size5, scope01, scope02, scope03, scope04,
						  scope05);
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

void llvm_after_call(int retInx, SCOPE retScope, KIND retType,
					 int64_t retValue) {
	DISPATCH_TO_OBSERVERS(after_call, retInx, retScope, retType, retValue)
}

void llvm_after_void_call() {
	DISPATCH_TO_OBSERVERS_NOARG(after_void_call)
}

void llvm_after_struct_call() {
	DISPATCH_TO_OBSERVERS_NOARG(after_struct_call)
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

void llvm_create_global_array(int valInx, uint64_t addr, uint32_t size,
							  KIND type) {
	DISPATCH_TO_OBSERVERS(create_global_array, valInx, addr, size, type)
}

void llvm_call(IID iid, bool nounwind, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call, iid, nounwind, type, inx)
}

void llvm_call_sin(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_sin, iid, nounwind, argIID, type, inx)
}

void llvm_call_acos(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_acos, iid, nounwind, argIID, type, inx)
}

void llvm_call_sqrt(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_sqrt, iid, nounwind, argIID, type, inx)
}

void llvm_call_fabs(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_fabs, iid, nounwind, argIID, type, inx)
}

void llvm_call_cos(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_cos, iid, nounwind, argIID, type, inx)
}

void llvm_call_log(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_log, iid, nounwind, argIID, type, inx)
}

void llvm_call_exp(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_exp, iid, nounwind, argIID, type, inx)
}

void llvm_call_floor(IID iid, bool nounwind, IID argIID, KIND type, int inx) {
	DISPATCH_TO_OBSERVERS(call_floor, iid, nounwind, argIID, type, inx)
}

void llvm_call_malloc(IID iid, bool nounwind, KIND type, int size, int inx,
					  uint64_t mallocAddress) {
	DISPATCH_TO_OBSERVERS(call_malloc, iid, nounwind, type, size, inx,
						  mallocAddress)
}

void llvm_vaarg() {
	DISPATCH_TO_OBSERVERS_NOARG(vaarg)
}

void llvm_landingpad() {
	DISPATCH_TO_OBSERVERS_NOARG(landingpad)
}
