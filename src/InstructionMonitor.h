/**
 * @file InstructionMonitor.h
 * brief
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

#ifndef INSTRUCTION_MONITOR_H_
#define INSTRUCTION_MONITOR_H_

#include "Common.h"
#include "InstructionObserver.h"
#include <vector>
#include <memory>

/*******************************************************************************************/

extern "C" {

	// ***** Binary Operations ***** //
	void llvm_add(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				  int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_fadd(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_sub(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				  int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_fsub(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_mul(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				  int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_fmul(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_udiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_sdiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_fdiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_urem(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_srem(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);
	void llvm_frem(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope,
				   int64_t lValue, int64_t rValue, KIND type, int inx);

	// ***** Bitwise Binary Operations ***** //
	void llvm_shl(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				  KIND type, int inx);
	void llvm_lshr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				   KIND type, int inx);
	void llvm_ashr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				   KIND type, int inx);
	void llvm_and_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				   KIND type, int inx);
	void llvm_or_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				  KIND type, int inx);
	void llvm_xor_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				   KIND type, int inx);

	// ***** Vector Operations ***** //
	void llvm_extractelement(IID iid, KVALUE* op1, KVALUE* op2, int x);
	void llvm_insertelement();
	void llvm_shufflevector();

	// ***** Aggregate Operations ***** //
	void llvm_extractvalue(IID iid, int inx, int opinx);
	void llvm_insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int x);

	// ***** Memory Access and Addressing Operations ***** //
	void llvm_allocax(IID iid, KIND kind, uint64_t size, int inx, uint64_t addr);
	void llvm_allocax_array(IID iid, KIND kind, uint64_t size, int inx,
							uint64_t addr);
	void llvm_allocax_struct(IID iid, uint64_t size, int inx, uint64_t addr);

	void llvm_load(IID iid, KIND kind, SCOPE opScope, int opInx, uint64_t opAddr,
				   bool loadGlobal, int loadInx, int inx);
	void llvm_load_struct(IID iid, KIND kind, KVALUE* op, int inx);

	void llvm_store(int pInx, SCOPE pScope, KIND srcKind, SCOPE srcScope,
					int srcInx, int64_t srcValue);
	void llvm_fence();
	void llvm_cmpxchg(IID iid, PTR addr, KVALUE* value1, KVALUE* value2, int x);
	void llvm_atomicrmw();

	void llvm_getelementptr(IID iid, int baseInx, SCOPE baseScope,
							uint64_t baseAddr, int offsetInx, int64_t offsetValue,
							KIND kind, uint64_t size, bool loadGlobal, int loadInx,
							int inx);
	void llvm_getelementptr_array(int baseInx, SCOPE baseScope, uint64_t baseAddr,
								  int elementSize, int scopeInx01, int scopeInx02,
								  int scopeInx03, int64_t valOrInx01,
								  int64_t valOrInx02, int64_t valOrInx03,
								  int size01, int size02, int inx);
	void llvm_getelementptr_struct(IID iid, int baseInx, SCOPE baseScope,
								   uint64_t baseAddr, int inx);

	// ***** Conversion Operations ***** //
	void llvm_trunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					int inx);
	void llvm_zext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				   int inx);
	void llvm_sext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
				   int inx);
	void llvm_fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					  int inx);
	void llvm_fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					int inx);
	void llvm_fptoui(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					 int inx);
	void llvm_fptosi(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					 int inx);
	void llvm_uitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					 int inx);
	void llvm_sitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					 int inx);
	void llvm_ptrtoint(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					   int inx);
	void llvm_inttoptr(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					   int inx);
	void llvm_bitcast(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					  int inx);

	// ***** Terminator Instructions ***** //
	void llvm_branch(IID iid, bool conditional, int valInx, SCOPE scope, KIND type,
					 uint64_t value);
	void llvm_branch2(IID iid, bool conditional);
	void llvm_indirectbr(IID iid, KVALUE* op1, int inx);
	void llvm_invoke(IID iid, KVALUE op, int inx);
	void llvm_resume(IID iid, KVALUE* op1, int inx);
	void llvm_return_(IID iid, int valInx, SCOPE scope, KIND type, int64_t value);
	void llvm_return2_(IID iid, int inx);
	void llvm_return_struct_(IID iid, int inx, int valInx);
	void llvm_switch_(IID iid, KVALUE* op, int inx);
	void llvm_unreachable();

	// ***** Other Operations ***** //
	void llvm_icmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				   KIND type, PRED pred, int inx);
	void llvm_fcmp(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
				   KIND type, PRED pred, int inx);
	void llvm_phinode(IID iid, int inx);
	void llvm_select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int x);
	void llvm_push_stack(int inx, SCOPE scope, KIND type, uint64_t addr);
	void llvm_push_string(int c);
	void llvm_push_phinode_constant_value(KVALUE* value, int blockId);
	void llvm_push_phinode_value(int valId, int blockId);
	void llvm_push_return_struct(KVALUE* value);
	void llvm_push_struct_type(KIND kind);
	void llvm_push_struct_element_size(uint64_t s);
	void llvm_push_getelementptr_inx(uint64_t value);
	void llvm_push_getelementptr_inx5(int scope01, int scope02, int scope03,
									  int scope04, int scope05, int64_t vori01,
									  int64_t vori02, int64_t vori03,
									  int64_t vori04, int64_t vori05);
	void llvm_push_getelementptr_inx2(int value);
	void llvm_push_array_size(uint64_t i);
	void llvm_push_array_size5(int scope01, int scope02, int scope03, int scope04,
							   int scope05);
	void llvm_construct_array_type(uint64_t i);
	void llvm_after_call(int retInx, SCOPE retScope, KIND retType,
						 int64_t retValue);
	void llvm_after_void_call();
	void llvm_after_struct_call();
	void llvm_create_stack_frame(int size);
	void llvm_create_global_symbol_table(int size);
	void llvm_record_block_id(int id);
	void llvm_create_global(KVALUE* value, KVALUE* initializer);
	void llvm_create_global_array(int valInx, uint64_t addr, uint32_t size,
								  KIND type);
	void llvm_call(IID iid, bool nounwind, KIND type, int x);
	void llvm_call_sin(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_acos(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_sqrt(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_fabs(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_cos(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_log(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_exp(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_floor(IID iid, bool nounwind, IID argIID, KIND type, int x);
	void llvm_call_malloc(IID iid, bool nounwind, KIND type, int size, int x,
						  uint64_t mallocAddress);
	void llvm_vaarg();
	void llvm_landingpad();
}

/*******************************************************************************************/
extern std::vector<std::unique_ptr<InstructionObserver>> observers_;
template <class T> class RegisterObserver {
public:
	RegisterObserver(std::string name) {
		DEBUG_STDERR(">>> Registering observer: " << name);

		observers_.push_back(std::unique_ptr<T>(new T(name)));
	}
};

#endif // INSTRUCTION_MONITOR_H_
