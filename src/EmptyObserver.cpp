/**
 * @file EmptyObserver.cpp
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

// Author: Cindy Rubio-Gonzalez

#include "Common.h"
#include "EmptyObserver.h"
#include <stack>

using namespace std;
	
void EmptyObserver::load(IID iid UNUSED, KIND type UNUSED, SCOPE opScope UNUSED, int opInx UNUSED, uint64_t opAddr UNUSED, bool loadGlobal UNUSED, int loadInx UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {
}

void EmptyObserver::load_struct(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, int file UNUSED, int line UNUSED, int inx UNUSED) {
}

// ***** Binary Operations ***** //

void EmptyObserver::add(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::fadd(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::sub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::fsub(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::mul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::fmul(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::udiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::sdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::fdiv(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::urem(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::srem(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::frem(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

// ***** Bitwise Binary Operations ***** //
void EmptyObserver::shl(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::lshr(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::ashr(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::and_(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::or_(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

void EmptyObserver::xor_(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, int file UNUSED, int line UNUSED, int col UNUSED, int inx UNUSED) {
}

// ***** Vector Operations ***** //
void EmptyObserver::extractelement(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {
}

void EmptyObserver::insertelement() {
}

void EmptyObserver::shufflevector() {
}


// ***** AGGREGATE OPERATIONS ***** //
void EmptyObserver::extractvalue(IID iid UNUSED, int inx UNUSED, int opinx UNUSED) {
}

void EmptyObserver::insertvalue(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {
}

// ***** Memory Access and Addressing Operations ***** //
void EmptyObserver::allocax(IID iid UNUSED, KIND kind UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, 
			    int valInx UNUSED, SCOPE scope UNUSED, KIND type UNUSED, uint64_t addr UNUSED) {
}

void EmptyObserver::allocax_array(IID iid UNUSED, KIND kind UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, 
				  int valInx UNUSED, SCOPE scope UNUSED, KIND type UNUSED, uint64_t addr UNUSED) {
}

void EmptyObserver::allocax_struct(IID iid UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, 
				   int valInx UNUSED, SCOPE scope UNUSED, KIND type UNUSED, uint64_t addr UNUSED) {
}

void EmptyObserver::store(int pInx UNUSED, SCOPE pScope UNUSED, KIND srcKind UNUSED, SCOPE srcScope UNUSED, int srcInx UNUSED, int64_t srcValue UNUSED,  int file UNUSED, int line UNUSED, int inx UNUSED) {
}

void EmptyObserver::fence() {
}

void EmptyObserver::cmpxchg(IID iid UNUSED, PTR addr UNUSED, KVALUE* kv1 UNUSED, KVALUE* kv2 UNUSED, int inx UNUSED) {
}

void EmptyObserver::atomicrmw() {
}

void EmptyObserver::getelementptr(IID iid UNUSED, bool inbound UNUSED, int baseInx UNUSED, SCOPE baseScope UNUSED, uint64_t baseAddr UNUSED, 
				  int offsetInx UNUSED, int64_t offsetValue UNUSED, 
				  KIND kind UNUSED, uint64_t size UNUSED, bool loadGlobal UNUSED, int loadInx UNUSED, int line UNUSED, int inx UNUSED) {
}

void EmptyObserver::getelementptr_array(int baseInx UNUSED, SCOPE baseScope UNUSED, uint64_t baseAddr UNUSED, 
					KIND kind UNUSED, int elementSize UNUSED, int scopeInx01 UNUSED, int scopeInx02 UNUSED, int scopeInx03 UNUSED, 
					int64_t valOrInx01 UNUSED, int64_t valOrInx02 UNUSED, int64_t valOrInx03 UNUSED, int size01 UNUSED, int size02 UNUSED,  int inx UNUSED) {
}

void EmptyObserver::getelementptr_struct(IID iid UNUSED, bool inbound UNUSED, int baseInx UNUSED, SCOPE baseScope UNUSED, uint64_t baseAddr UNUSED, 
					 KIND kind UNUSED, KIND arrayKind UNUSED, int inx UNUSED) {
}

// ***** Conversion Operations ***** //

void EmptyObserver::trunc(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::zext(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::sext(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::fptrunc(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::fpext(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::fptoui(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::fptosi(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::uitofp(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::sitofp(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::ptrtoint(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::inttoptr(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

void EmptyObserver::bitcast(int64_t op UNUSED, SCOPE opScope UNUSED, KIND opKind UNUSED, KIND kind UNUSED, int size UNUSED, int inx UNUSED) {
}

// ***** TerminatorInst ***** //
void EmptyObserver::branch(IID iid UNUSED, bool conditional UNUSED, int valInx UNUSED, SCOPE scope UNUSED, KIND type UNUSED, uint64_t value UNUSED, int inx UNUSED) {
}

void EmptyObserver::branch2(IID iid UNUSED, bool conditional UNUSED, int inx UNUSED) {
}

void EmptyObserver::indirectbr(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {
}

void EmptyObserver::invoke(IID iid UNUSED, KVALUE* call_value UNUSED, int inx UNUSED) {
}

void EmptyObserver::resume(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {
}

void EmptyObserver::return_(IID iid UNUSED, int valInx UNUSED, SCOPE scope UNUSED, KIND type UNUSED, int64_t value UNUSED, int inx UNUSED) {
}

void EmptyObserver::return2_(IID iid UNUSED, int inx UNUSED) {
}

void EmptyObserver::return_struct_(IID iid UNUSED, int inx UNUSED, int valInx UNUSED) {
}

void EmptyObserver::switch_(IID iid UNUSED, KVALUE* op UNUSED, int inx UNUSED) {
}

void EmptyObserver::unreachable() {
}

// ***** Other Operations ***** //
void EmptyObserver::icmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, PRED pred UNUSED, int line UNUSED, int inx UNUSED) {
}

void EmptyObserver::fcmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue UNUSED, int64_t rValue UNUSED, KIND type UNUSED, PRED pred UNUSED, int line UNUSED, int inx UNUSED) {
}

void EmptyObserver::phinode(IID iid UNUSED, int inx UNUSED) {
}

void EmptyObserver::select(IID iid UNUSED, KVALUE* cond UNUSED, KVALUE* tvalue UNUSED, KVALUE* fvalue UNUSED, int inx UNUSED) {
}

void EmptyObserver::push_string(int c UNUSED) {
}

void EmptyObserver::push_stack(int inx UNUSED, SCOPE scope UNUSED, KIND type UNUSED, uint64_t addr UNUSED) {
}

void EmptyObserver::push_phinode_constant_value(KVALUE* value UNUSED, int blockId UNUSED) {
}

void EmptyObserver::push_phinode_value(int valId UNUSED, int blockId UNUSED) {
}

void EmptyObserver::push_return_struct(KVALUE* value UNUSED) {
}

void EmptyObserver::push_struct_type(KIND kind UNUSED) {
}

void EmptyObserver::push_struct_element_size(uint64_t s UNUSED) {
}

void EmptyObserver::push_getelementptr_inx(KVALUE* value UNUSED) {
}

void EmptyObserver::push_getelementptr_inx5(int scope01 UNUSED, int scope02 UNUSED, int scope03 UNUSED, int scope04 UNUSED, int scope05 UNUSED, int64_t vori01 UNUSED, int64_t vori02 UNUSED, int64_t vori03 UNUSED, int64_t vori04 UNUSED, int64_t vori05 UNUSED) {
}

void EmptyObserver::push_getelementptr_inx2(int value UNUSED) {
}

void EmptyObserver::push_array_size(uint64_t i UNUSED) {
}

void EmptyObserver::push_array_size5(int s1 UNUSED, int s2 UNUSED, int s3 UNUSED, int s4 UNUSED, int s5 UNUSED){
}

void EmptyObserver::construct_array_type(uint64_t i UNUSED) {
}

void EmptyObserver::after_call(int retInx UNUSED, SCOPE retScope UNUSED, KIND retType UNUSED, int64_t retValue UNUSED, int line UNUSED) {
}

void EmptyObserver::after_void_call() {
}

void EmptyObserver::after_struct_call() {
}

void EmptyObserver::create_stack_frame(int size UNUSED) {
}

void EmptyObserver::create_global_symbol_table(int size UNUSED) {
}

void EmptyObserver::record_block_id(int id UNUSED) {
}

void EmptyObserver::create_global(KVALUE* kvalue UNUSED, KVALUE* initializer UNUSED) {
}

void EmptyObserver::create_global_array(KVALUE *value UNUSED, uint32_t size UNUSED, KIND type UNUSED) {
}

void EmptyObserver::call(IID iid UNUSED, bool nounwind UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::call_sin(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::call_acos(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::call_sqrt(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::call_cos(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::call_log(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::call_floor(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::call_fabs(IID iid UNUSED, bool nounwind UNUSED, int pc UNUSED, KIND type UNUSED, int inx UNUSED) {
}

void EmptyObserver::vaarg() {
}


void EmptyObserver::landingpad() {
}

