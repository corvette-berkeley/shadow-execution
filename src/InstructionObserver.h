/**
 * @file InstructionObserver.h"
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

#ifndef INSTRUCTION_OBSERVER_H_
#define INSTRUCTION_OBSERVER_H_

#include "Common.h"
#define UNUSED __attribute__((__unused__))

/**
 * Interface for observers. 
 *
 * Method can be overrided to implement analysis of interest. All methods have
 * default empty impelementations.
 */
class InstructionObserver {
public:
	InstructionObserver(std::string name) : name_(name) {}
	virtual ~InstructionObserver() {}

	// ***** Binary Operators **** //
	virtual void add(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void fadd(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void sub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void fsub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void mul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void fmul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void udiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void sdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void fdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void urem(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void srem(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void frem(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};

	// ***** Bitwise Binary Operators ***** //
	virtual void shl(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void lshr(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void ashr(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void and_(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void or_(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};
	virtual void xor_(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {};

	// ***** Vector Operations ***** //
	virtual void extractelement(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void insertelement() {};
	virtual void shufflevector() {};

	// ***** Aggregate Operations ***** //
	virtual void extractvalue(IID iid UNUSED, int inx UNUSED, int opinx UNUSED) {};
	virtual void insertvalue(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};

	// ***** Memory Access and Addressing Operations ***** //
	virtual void allocax(IID iid UNUSED, KIND kind UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* result UNUSED) {};
	virtual void allocax_array(IID iid UNUSED, KIND kind UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* addr UNUSED) {};
	virtual void allocax_struct(IID iid UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* addr UNUSED) {};
	virtual void load(IID iid UNUSED, KIND kind UNUSED, SCOPE opScope UNUSED, int opInx UNUSED, uint64_t opAddr UNUSED,  bool loadGlobal UNUSED, int loadInx UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {};
	virtual void load_struct(IID iid UNUSED, KIND kind UNUSED, KVALUE* op UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {};
	virtual void store(int pInx UNUSED, SCOPE pScope UNUSED, KIND srcKind UNUSED, SCOPE srcScope UNUSED, int srcInx UNUSED, int64_t srcValue UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {};
	virtual void fence() {};
	virtual void cmpxchg(IID iid UNUSED, PTR addr UNUSED, KVALUE* value1 UNUSED, KVALUE* value2 UNUSED, int inx UNUSED) {};
	virtual void atomicrmw() {};
	virtual void getelementptr(IID iid UNUSED, bool inbound UNUSED, KVALUE* value UNUSED, KVALUE* index UNUSED, KIND kind UNUSED, 
				   uint64_t size UNUSED, bool loadGlobal UNUSED, int loadInx UNUSED, int line UNUSED, int inx UNUSED) {};
  virtual void getelementptr_array(KVALUE* value UNUSED, KIND kind UNUSED, int
      elementSize UNUSED, int scopeInx01 UNUSED, int scopeInx02 UNUSED, int
      scopeInx03 UNUSED, int64_t valOrInx01 UNUSED, int64_t valOrInx02 UNUSED,
      int64_t valOrInx03 UNUSED, int size01 UNUSED, int size02 UNUSED,  int inx
      UNUSED) {};
  virtual void getelementptr_struct(IID iid UNUSED, bool inbound UNUSED, KVALUE* value UNUSED, KIND kind UNUSED, KIND arrayKind UNUSED, int inx UNUSED) {};

	// ***** Conversion Operations ***** //
	virtual void trunc(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void zext(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void sext(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void fptrunc(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void fpext(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void fptoui(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void fptosi(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void uitofp(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void sitofp(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void ptrtoint(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void inttoptr(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};
	virtual void bitcast(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {};

	// ***** Terminator Instructions *****/
	virtual void branch(IID iid UNUSED, bool conditional UNUSED,  KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void branch2(IID iid UNUSED, bool conditional UNUSED, int inx UNUSED) {};
	virtual void indirectbr(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void invoke(IID iid UNUSED, KVALUE* op UNUSED, int inx UNUSED) {};
	virtual void resume(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void return_(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void return2_(IID iid UNUSED, int inx UNUSED) {};
	virtual void return_struct_(IID iid UNUSED, int inx UNUSED, int valInx UNUSED) {};
	virtual void switch_(IID iid UNUSED, KVALUE* op UNUSED, int inx UNUSED) {};
	virtual void unreachable() {};

	// ***** Other Operations ***** //
	virtual void icmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, PRED pred UNUSED, int line UNUSED, int inx UNUSED) {};
	virtual void fcmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, PRED pred UNUSED, int line UNUSED, int inx UNUSED) {};
	virtual void phinode(IID iid UNUSED, int inx UNUSED) {};
	virtual void select(IID iid UNUSED, KVALUE* cond UNUSED, KVALUE* tvalue UNUSED, KVALUE* fvealue UNUSED, int inx UNUSED) {};
	virtual void push_string(int c UNUSED){};
	virtual void push_stack(KVALUE* value UNUSED) {};
	virtual void push_phinode_value(int valId UNUSED, int blockId UNUSED) {};
	virtual void push_phinode_constant_value(KVALUE* value UNUSED, int blockId UNUSED) {};
	virtual void push_return_struct(KVALUE* value UNUSED) {};
	virtual void push_struct_type(KIND kind UNUSED) {};
	virtual void push_struct_element_size(uint64_t s UNUSED) {};
	virtual void push_getelementptr_inx(KVALUE* value UNUSED) {};
	virtual void push_getelementptr_inx5(int scope01 UNUSED, int scope02 UNUSED, int scope03 UNUSED, int scope04 UNUSED, int scope05 UNUSED, int64_t vori01 UNUSED, 
					     int64_t vori02 UNUSED, int64_t vori03 UNUSED, int64_t vori04 UNUSED, int64_t vori05 UNUSED) {};
	virtual void push_getelementptr_inx2(int value UNUSED) {};
	virtual void push_array_size(uint64_t i UNUSED) {};
	virtual void push_array_size5(int scope01 UNUSED, int scope02 UNUSED, int scope03 UNUSED, int scope04 UNUSED, int scope05 UNUSED) {}; 
	virtual void construct_array_type(uint64_t i UNUSED) {};
	virtual void after_call(KVALUE* value UNUSED, int line UNUSED) {};
	virtual void after_void_call() {};
	virtual void after_struct_call() {};
	virtual void create_stack_frame(int size UNUSED) {};
	virtual void create_global_symbol_table(int size UNUSED) {};
	virtual void record_block_id(int id UNUSED) {};
	virtual void create_global(KVALUE* value UNUSED, KVALUE* initializer UNUSED) {};
	virtual void create_global_array(KVALUE *value UNUSED, uint32_t size UNUSED, KIND type UNUSED) {};
	virtual void call(IID iid UNUSED, bool nounwind UNUSED, KIND type UNUSED, int inx UNUSED) {};
	virtual void call_sin(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {};
	virtual void call_acos(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {};
	virtual void call_sqrt(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {};
	virtual void call_fabs(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {};
	virtual void call_cos(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {};
	virtual void call_malloc(IID iid UNUSED, bool nounwind UNUSED, KIND type UNUSED, KVALUE* value UNUSED, 
				 int size UNUSED, int inx UNUSED, KVALUE* mallocAddress UNUSED) {};
	virtual void vaarg() {};
	virtual void landingpad() {};

private:
	std::string name_;
};


#endif /* INSTRUCTIONOBSERVER_H_ */
