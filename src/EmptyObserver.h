/**
 * @file EmptyObserver.h
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

// Author: Cindy Rubio-Gonzalez

#ifndef EMPTY_OBSERVER_H_
#define EMPTY_OBSERVER_H_

#include "InstructionObserver.h"
#include <stack>

using namespace std;

/*******************************************************************************************/
// helpful macros for defining instrumenters
#define DEFAULT_CONSTRUCTOR(T)                                                 \
  T(std::string name) : InstructionObserver(name) {}

/*******************************************************************************************/

class EmptyObserver : public InstructionObserver {
public:
	DEFAULT_CONSTRUCTOR(EmptyObserver);

	virtual void load(IID iid, KIND kind, SCOPE opScope, int opInx,
					  uint64_t opAddr, bool loadGlobal, int loadInx, int inx);

	virtual void load_struct(IID iid, KIND kind, KVALUE* op, int inx);

	// ***** Binary Operations ***** //

	virtual void add(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					 int64_t rValue, KIND type, int inx);

	virtual void fadd(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void sub(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					 int64_t rValue, KIND type, int inx);

	virtual void fsub(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void mul(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					 int64_t rValue, KIND type, int inx);

	virtual void fmul(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void udiv(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void sdiv(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void fdiv(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void urem(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void srem(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	virtual void frem(IID iid, SCOPE lScope, SCOPE rScope, int64_t lValue,
					  int64_t rValue, KIND type, int inx);

	// ***** Bitwise Binary Operations ***** //
	virtual void shl(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
					 KIND type, int inx);

	virtual void lshr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
					  KIND type, int inx);

	virtual void ashr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
					  KIND type, int inx);

	virtual void and_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
					  KIND type, int inx);

	virtual void or_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
					 KIND type, int inx);

	virtual void xor_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
					  KIND type, int inx);

	// ***** Vector Operations ***** //
	virtual void extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx);

	virtual void insertelement();

	virtual void shufflevector();

	// ***** AGGREGATE OPERATIONS ***** //
	virtual void extractvalue(IID iid, int inx, int opinx);

	virtual void insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx);

	// ***** Memory Access and Addressing Operations ***** //
	virtual void allocax(IID iid, KIND kind, uint64_t size, int inx,
						 uint64_t addr);

	virtual void allocax_array(IID iid, KIND kind, uint64_t size, int inx,
							   uint64_t addr);

	virtual void allocax_struct(IID iid, uint64_t size, int inx, uint64_t addr);

	virtual void store(int pInx, SCOPE pScope, KIND srcKind, SCOPE srcScope,
					   int srcInx, int64_t srcValue);

	virtual void fence();

	virtual void cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx);

	virtual void atomicrmw();

	virtual void getelementptr(IID iid, int baseInx, SCOPE baseScope,
							   uint64_t baseAddr, int offsetInx,
							   int64_t offsetValue, KIND kind, uint64_t size,
							   bool loadGlobal, int loadInx, int inx);

	virtual void getelementptr_array(int baseInx, SCOPE baseScope,
									 uint64_t baseAddr, int elementSize,
									 int scopeInx01, int scopeInx02,
									 int scopeInx03, int64_t valOrInx01,
									 int64_t valOrInx02, int64_t valOrInx03,
									 int size01, int size02, int inx);

	virtual void getelementptr_struct(IID iid, int baseInx, SCOPE baseScope,
									  uint64_t baseAddr, int inx);

	// ***** Conversion Operations ***** //
	virtual void trunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
					   int size, int inx);

	virtual void zext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					  int inx);

	virtual void sext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size,
					  int inx);

	virtual void fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						 int size, int inx);

	virtual void fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
					   int size, int inx);

	virtual void fptoui(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						int size, int inx);

	virtual void fptosi(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						int size, int inx);

	virtual void uitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						int size, int inx);

	virtual void sitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						int size, int inx);

	virtual void ptrtoint(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						  int size, int inx);

	virtual void inttoptr(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						  int size, int inx);

	virtual void bitcast(int64_t op, SCOPE opScope, KIND opKind, KIND kind,
						 int size, int inx);

	// ***** TerminatorInst ***** //
	virtual void branch(IID iid, bool conditional, int valInx, SCOPE scope,
						KIND type, uint64_t value);

	virtual void branch2(IID iid, bool conditional);

	virtual void indirectbr(IID iid, KVALUE* op1, int inx);

	virtual void invoke(IID iid, KVALUE* call_value, int inx);

	virtual void resume(IID iid, KVALUE* op1, int inx);

	virtual void return_(IID iid, int valInx, SCOPE scope, KIND type,
						 int64_t value);

	virtual void return2_(IID iid, int inx);

	virtual void return_struct_(IID iid, int inx, int valInx);

	virtual void switch_(IID iid, KVALUE* op, int inx);

	virtual void unreachable();

	// ***** Other Operations ***** //
	virtual void icmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue,
					  int64_t rValue, KIND type, PRED pred, int inx);

	virtual void fcmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue,
					  int64_t rValue, KIND type, PRED pred, int inx);

	virtual void phinode(IID iid, int inx);

	virtual void select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue,
						int inx);

	virtual void call(IID iid, bool nounwind, KIND type, int inx);

	virtual void call_malloc(IID iid, bool nounwind, KIND type, int size, int inx,
							 uint64_t mallocAddress);

	virtual void call_sin(IID iid, bool nounwind, int pc, KIND type, int inx);

	virtual void call_acos(IID iid, bool nounwind, int pc, KIND type, int inx);

	virtual void call_sqrt(IID iid, bool nounwind, int pc, KIND type, int inx);

	virtual void call_cos(IID iid, bool nounwind, int pc, KIND type, int inx);

	virtual void call_fabs(IID iid, bool nounwind, int pc, KIND type, int inx);

	virtual void call_log(IID iid, bool nounwind, int pc, KIND type, int inx);

	virtual void call_floor(IID iid, bool nounwind, int pc, KIND type, int inx);

	virtual void vaarg();

	virtual void landingpad();

	void push_string(int c);

	void push_stack(int inx, SCOPE scope, KIND type, uint64_t addr);

	void push_phinode_value(int valId, int blockId);

	void push_phinode_constant_value(KVALUE* value, int blockId);

	void push_return_struct(KVALUE* value);

	void push_struct_type(KIND kind);

	void push_struct_element_size(uint64_t size);

	void push_getelementptr_inx(uint64_t value);

	void push_getelementptr_inx5(int scope01, int scope02, int scope03,
								 int scope04, int scope05, int64_t vori01,
								 int64_t vori02, int64_t vori03, int64_t vori04,
								 int64_t vori05);

	void push_array_size(uint64_t size);

	void push_array_size5(int scope01, int scope02, int scope03, int scope04,
						  int scope05);

	void push_getelementptr_inx2(int value);

	void construct_array_type(uint64_t i);

	void after_call(int retInx, SCOPE retScope, KIND retType, int64_t retValue);

	void after_void_call();

	void after_struct_call();

	void create_stack_frame(int size);

	void create_global_symbol_table(int size);

	void record_block_id(int id);

	void create_global(KVALUE* value, KVALUE* initializer);

	void create_global_array(int valInx, uint64_t addr, uint32_t size, KIND type);
};

#endif /* EMPTY_OBSERVER_H_ */
