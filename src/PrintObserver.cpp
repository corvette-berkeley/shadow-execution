/**
 * @file PrintObserver.cpp
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

#include "Common.h"
#include "PrintObserver.h"
#include <stack>

using namespace std;
	
void PrintObserver::load(IID iid, KIND type, SCOPE opScope, int opInx, uint64_t opAddr, bool loadGlobal, int loadInx, int file, int line, int inx) {
  DEBUG_STDOUT("<<<<< LOAD >>>>> " << IID_ToString(iid).c_str() <<
      ", kind:" << KIND_ToString(type).c_str() << ", opScope:" << SCOPE_ToString(opScope) << ", opInx:" << opInx << ", opAddr:" << opAddr << ", loadGlobal: " << loadGlobal
      << ", loadInx: " << loadInx << ", file: " << file << ", line:"
      << line << ", [INX: " << inx << "]");
}

void PrintObserver::load_struct(IID iid, KIND type, KVALUE* op, int file, int line, int inx) {
  DEBUG_STDOUT("<<<<< LOAD STRUCT >>>>> " <<
      IID_ToString(iid).c_str() << ", kind:" <<
      KIND_ToString(type).c_str() << ", " <<
      KVALUE_ToString(op).c_str() << ", file: " << file << ", line:"
      << line << ", [INX: " << inx << "]");
}

// ***** Binary Operations ***** //

void PrintObserver::add(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< ADD >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< FADD >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::sub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< SUB >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< FSUB >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::mul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< MUL >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< FMUL >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::udiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< UDIV >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::sdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< SDIV >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< FDIV >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::urem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< UREM >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::srem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< SREM >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::frem(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< FREM >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

// ***** Bitwise Binary Operations ***** //
void PrintObserver::shl(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< SHL >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::lshr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< LSHR >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::ashr(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< ASHR >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::and_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< AND >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::or_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< OR >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

void PrintObserver::xor_(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file UNUSED, int line, int col UNUSED, int inx) {
  DEBUG_STDOUT("<<<<< XOR >>>>> lscope:" << SCOPE_ToString(lScope) << ", rscope:" << SCOPE_ToString(rScope) << ", lvalue:" << lValue << ", rValue:" << rValue << ", type:" << KIND_ToString(type) << ", line:" << line << ", [INX:" << inx << "]");
}

// ***** Vector Operations ***** //

void PrintObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< EXTRACTELEMENT >>>>>" << IID_ToString(iid).c_str() << ", vector:" << KVALUE_ToString(op1).c_str() << ", index:" << KVALUE_ToString(op2) << ", [INX: " << inx << "]"); 
}

void PrintObserver::insertelement() {
  DEBUG_STDOUT("<<<<< INSERTELEMENT >>>>>");
}

void PrintObserver::shufflevector() {
  DEBUG_STDOUT("<<<<< SHUFFLEVECTOR >>>>>");
}


// ***** AGGREGATE OPERATIONS ***** //

void PrintObserver::extractvalue(IID iid, int inx, int opinx) {
  DEBUG_STDOUT("<<<<< EXTRACTVALUE >>>>> " << IID_ToString(iid).c_str() << ", agg_inx:" << opinx << ", [INX: " << inx << "]"); 
}

void PrintObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< INSERTVALUE >>>>>" << IID_ToString(iid).c_str() << ", vector:" << KVALUE_ToString(op1).c_str() << ", index:" << KVALUE_ToString(op2) << ", [INX: " << inx << "]"); 
}

// ***** Memory Access and Addressing Operations ***** //

void PrintObserver::allocax(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, int valInx, SCOPE scope, KIND type, uint64_t addr) {
  DEBUG_STDOUT("<<<<< ALLOCA >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", arg:" << arg << ", line:" << line 
	       << ", result:" << "inx: " << valInx << ", scope: " << SCOPE_ToString(scope) << ", type: " << KIND_ToString(type) << ", value: " << (void*)addr 
	       << ", [INX: " << inx << "]");
}

void PrintObserver::allocax_array(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, int valInx, SCOPE scope, KIND type, uint64_t addr) {
  DEBUG_STDOUT("<<<<< ALLOCA >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", arg:" << arg << ", line:" << line 
	       << "inx: " << valInx << ", scope: " << SCOPE_ToString(scope) << ", type: " << KIND_ToString(type) << ", value: " << (void*)addr  
	       << ", [INX: " << inx << "]");
}

void PrintObserver::allocax_struct(IID iid, uint64_t size, int inx, int line, bool arg, int valInx, SCOPE scope, KIND type, uint64_t addr) {
  DEBUG_STDOUT("<<<<< ALLOCA >>>>> " << IID_ToString(iid) << ", size:" << size << ", arg:" << arg << ", line:" << line 
	       << "inx: " << valInx << ", scope: " << SCOPE_ToString(scope) << ", type: " << KIND_ToString(type) << ", value: " << (void*)addr 
	       << ", [INX: " << inx << "]");
}

void PrintObserver::store(int pInx, SCOPE pScope, KIND srcKind, SCOPE srcScope, int srcInx, int64_t srcValue,  int file, int line, int inx) {
  DEBUG_STDOUT("<<<<< STORE >>>>> pInx:" << pInx << ", pScope:" << SCOPE_ToString(pScope) << ", srcKind:" << KIND_ToString(srcKind) << ", srcScope:" << SCOPE_ToString(srcScope) << ", srcInx:" << srcInx << ", srcValue:" << srcValue << ", file:" << file << ", line:" << line << ", [INX: " << inx << "]");
}

void PrintObserver::fence() {
  DEBUG_STDOUT("<<<<< FENCE >>>>>");
}

void PrintObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx) {
  DEBUG_STDOUT("<<<<< CMPXCHG >>>>> " << IID_ToString(iid) << ", address:" << PTR_ToString(addr) << ", operand 1:" << KVALUE_ToString(kv1) << ", operand 2:" << KVALUE_ToString(kv2) << ", [INX: " << inx << "]");
}

void PrintObserver::atomicrmw() {
  DEBUG_STDOUT("<<<<< ATOMICRMW >>>>>");
}

void PrintObserver::getelementptr(IID iid, bool inbound, int baseInx UNUSED, SCOPE baseScope UNUSED, uint64_t baseAddr UNUSED, 
				  int offsetInx UNUSED, int64_t offsetValue UNUSED, 
				  KIND kind, uint64_t size, bool loadGlobal, int loadInx, int line, int inx) {
  DEBUG_STDOUT("<<<<< GETELEMENTPTR >>>>> " << IID_ToString(iid) << ", inbound" << (inbound ? "1" : "0") << ", pointer value: TOPRINT" << ", offsetValue: TOPRINT" << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", loadGlobal:" << loadGlobal << ", loadInx:" << loadInx << ", line:" << line << " [INX: " << inx << "]");
}

void PrintObserver::getelementptr_array(int baseInx UNUSED, SCOPE baseScope UNUSED, uint64_t baseAddr UNUSED, KIND kind, int elementSize, 
					int scopeInx01, int scopeInx02, int scopeInx03, int64_t valOrInx01, int64_t valOrInx02, int64_t valOrInx03, int size01, int size02,  int inx) {
  DEBUG_STDOUT("<<<<< GETELEMENTPTR ARRAY >>>>> pointer value: TOPRINT" << ", elementSize:" << elementSize <<  ", kind:" << KIND_ToString(kind) << ", scopeInx01:" << scopeInx01 << ", scopeInx02:" << scopeInx02 << ", scopeInx03:" << scopeInx03 << ", valOrInx01:" << valOrInx01 << ", valOrInx02:" << valOrInx02 << ", valOrInx03:" << valOrInx03 << ", size01:" << size01 << ", size02:" << size02 << " [INX: " << inx << "]");
}

void PrintObserver::getelementptr_struct(IID iid, bool inbound, int baseInx UNUSED, SCOPE baseScope UNUSED, uint64_t baseAddr UNUSED, KIND kind, KIND arrayKind, int inx) {
  DEBUG_STDOUT("<<<<< GETELEMENTPTR STRUCT >>>>> " << IID_ToString(iid) << ", inbound" << (inbound ? "1" : "0") << ", pointer value: TOPRINT" << ", kind:" << KIND_ToString(kind) << ", arrayKind:" << KIND_ToString(arrayKind) << " [INX: " << inx << "]");
}

// ***** Conversion Operations ***** //

void PrintObserver::trunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< TRUNC >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::zext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< ZEXT >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::sext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< SEXT >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< FPTRUNC >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< FPEXT >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::fptoui(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< FPTOUI >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::fptosi(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< FPTOSI >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::uitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< UITOFP >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::sitofp(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< SITOFP >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::ptrtoint(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< PTRTOINT >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::inttoptr(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< INTTOPTR >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

void PrintObserver::bitcast(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx) {
  DEBUG_STDOUT("<<<<< BITCAST >>>>> op:" << op << ", scope:" << SCOPE_ToString(opScope) << ", opKind:" << KIND_ToString(opKind) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", [INX: " << inx << "]");
}

// ***** TerminatorInst ***** //
void PrintObserver::branch(IID iid, bool conditional, int valInx, SCOPE scope UNUSED, KIND type UNUSED, uint64_t value, int inx) {
  DEBUG_STDOUT("<<<<< BRANCH >>>>> " << IID_ToString(iid) << ", cond:" << (conditional ? "1" : "0") << ", condition value:" 
	       << "inx: " << valInx << ", scope: " << SCOPE_ToString(scope) << ", type: " << KIND_ToString(type) << ", value: " << value << " [INX: " << inx << "]");
}


void PrintObserver::branch2(IID iid, bool conditional, int inx) {
  DEBUG_STDOUT("<<<<< BRANCH [GOTO] >>>>> " << IID_ToString(iid) << ", cond:" << (conditional ? "1" : "0") << " [INX: " << inx << "]");
}

void PrintObserver::indirectbr(IID iid, KVALUE* op1, int inx) {
  DEBUG_STDOUT("<<<<< INDIRECTBR >>>>> " << IID_ToString(iid) << ", condition value:" << KVALUE_ToString(op1) << " [INX: " << inx << "]");
}

void PrintObserver::invoke(IID iid, KVALUE* call_value, int inx) {
  DEBUG_STDOUT("<<<<< INVOKE >>>>> " << IID_ToString(iid) << ", call value:" << KVALUE_ToString(call_value) << " [INX: " << inx << "]");
}

void PrintObserver::resume(IID iid, KVALUE* op1, int inx) {
  DEBUG_STDOUT("<<<<< RESUME >>>>> " << IID_ToString(iid) << ", acc value:" << KVALUE_ToString(op1) << " [INX: " << inx << "]");
}

void PrintObserver::return_(IID iid, int valInx, SCOPE scope, KIND type, int64_t value, int inx) {
  DEBUG_STDOUT("<<<<< RETURN >>>>> " << IID_ToString(iid) << ",inx: " << valInx << ", scope: " << SCOPE_ToString(scope) << ", type: " << KIND_ToString(type) << ", value: " << value << " [INX: " << inx << "]");
}

void PrintObserver::return2_(IID iid, int inx) {
  DEBUG_STDOUT("<<<<< RETURN VOID >>>>> " << IID_ToString(iid) << " [INX: " << inx << "]");
}

void PrintObserver::return_struct_(IID iid, int inx, int valInx) {
  DEBUG_STDOUT("<<<<< RETURN STRUCT >>>>> " << IID_ToString(iid) << ", index:" << valInx << " [INX: " << inx << "]");
}

void PrintObserver::switch_(IID iid, KVALUE* op, int inx) {
  DEBUG_STDOUT("<<<<< SWITCH >>>>> " << IID_ToString(iid) << ", condition:" << KVALUE_ToString(op) << " [INX: " << inx << "]");
}

void PrintObserver::unreachable() {
  DEBUG_STDOUT("<<<<< UNREACHABLE >>>>>");
}

// ***** Other Operations ***** //

void PrintObserver::icmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue, int64_t rValue, KIND type, PRED pred, int line, int inx) {
  DEBUG_STDOUT("<<<<< ICMP >>>>> " << ", lScope:" << SCOPE_ToString(lScope) << ", rScope: " << SCOPE_ToString(rScope) << ", lValue:" << lValue << ", rValue:" << rValue << ", kind:" << KIND_ToString(type) << ", pred:" << pred << ", line:" << line << " [INX: " << inx << "]");
}

void PrintObserver::fcmp(SCOPE lScope UNUSED, SCOPE rScope UNUSED, int64_t lValue, int64_t rValue, KIND type, PRED pred, int line, int inx) {
  DEBUG_STDOUT("<<<<< FCMP >>>>> " << ", lScope:" << SCOPE_ToString(lScope) << ", rScope: " << SCOPE_ToString(rScope) << ", lValue:" << lValue << ", rValue:" << rValue << ", kind:" << KIND_ToString(type) << ", pred:" << pred << ", line:" << line << " [INX: " << inx << "]");
}

void PrintObserver::phinode(IID iid, int inx) {
  DEBUG_STDOUT("<<<<< PHINODE >>>>> " << IID_ToString(iid) << " [INX: " << inx << "]");
}

void PrintObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {
  DEBUG_STDOUT("<<<<< SELECT >>>>> " << IID_ToString(iid) << ", condition:" << KVALUE_ToString(cond) << ", true value:" << KVALUE_ToString(tvalue) << ", false value:" << KVALUE_ToString(fvalue) << " [INX: " << inx << "]");
}

void PrintObserver::push_string(int c) {
  DEBUG_STDOUT("<<<<< PUSH STRING >>>>> character: " << (char)(((int)'0')+c));
}

void PrintObserver::push_stack(int inx UNUSED, SCOPE scope UNUSED, KIND type UNUSED, uint64_t addr UNUSED) {
  DEBUG_STDOUT("<<<<< PUSH VALUE >>>>> value: TOPRINT");
}

void PrintObserver::push_phinode_constant_value(KVALUE* value, int blockId) {
  DEBUG_STDOUT("<<<<< PUSH PHINODE CONSTANT VALUE >>>>> value: " << KVALUE_ToString(value) << ", block id:" << blockId);
}

void PrintObserver::push_phinode_value(int valId, int blockId) {
  DEBUG_STDOUT("<<<<< PUSH PHINODE VALUE >>>>> value id: " << valId << ", block id:" << blockId);
}

void PrintObserver::push_return_struct(KVALUE* value) {
  DEBUG_STDOUT("<<<<< PUSH RETURN STRUCT >>>>> value: " << KVALUE_ToString(value));
}

void PrintObserver::push_struct_type(KIND kind) {
  DEBUG_STDOUT("<<<<< PUSH STRUCT TYPE >>>>> kind: " << KIND_ToString(kind));
}

void PrintObserver::push_struct_element_size(uint64_t s) {
  DEBUG_STDOUT("<<<<< PUSH STRUCT ELEMENT SIZE >>>>> size: " << s);
}

void PrintObserver::push_getelementptr_inx(uint64_t value) {
  DEBUG_STDOUT("<<<<< PUSH GETELEMENTPTR INX >>>>> value: " << value);
}

void PrintObserver::push_getelementptr_inx5(int scope01, int scope02, int scope03, int scope04, int scope05, int64_t vori01, int64_t vori02, int64_t vori03, int64_t vori04, int64_t vori05) {
  DEBUG_STDOUT("<<<<< PUSH GETELEMENTPTR INX 5 >>>>> scope: [" << scope01 << ", " << scope02 << ", " << scope03 << ", " << scope04 << ", " << scope05 << ", value or index: [" << vori01 << ", " << vori02 << ", " << vori03 << ", " << vori04 << ", " << vori05);
}

void PrintObserver::push_getelementptr_inx2(int value) {
  DEBUG_STDOUT("<<<<< PUSH GETELEMENTPTR INX  2>>>>> value: " << value);
}

void PrintObserver::push_array_size(uint64_t i) {
  DEBUG_STDOUT("<<<<< PUSH ARRAY SIZE >>>>> size: " << i);
}

void PrintObserver::push_array_size5(int s1, int s2, int s3, int s4, int s5){
  DEBUG_STDOUT("<<<<< PUSH ARRAY SIZE  5 >>>>> size: [" << s1 << ", " << s2 << ", " << s3 << ", " << s4 << ", " << s5 << "]");
}

void PrintObserver::construct_array_type(uint64_t i) {
  DEBUG_STDOUT("<<<<< CONSTRUCT ARRAY TYPE >>>>> size: " << i);
}

void PrintObserver::after_call(int retInx UNUSED, SCOPE retScope UNUSED, KIND retType UNUSED, int64_t retValue UNUSED, int line) {
  DEBUG_STDOUT("<<<<< AFTER CALL >>>>> value: TOPRINT" << ", line: " << line);
}

void PrintObserver::after_void_call() {
  DEBUG_STDOUT("<<<<< AFTER VOID CALL >>>>>");
}

void PrintObserver::after_struct_call() {
  DEBUG_STDOUT("<<<<< AFTER STRUCT CALL >>>>>");
}

void PrintObserver::create_stack_frame(int size) {
  DEBUG_STDOUT("<<<<< CREATE STACK >>>>> size: " << size);
}

void PrintObserver::create_global_symbol_table(int size) {
  DEBUG_STDOUT("<<<<< CREATE SYMBOL TABLE >>>>> size: " << size);
}

void PrintObserver::record_block_id(int id) {
  DEBUG_STDOUT("<<<<< RECORD BLOCK ID >>>> id:" << id);
}

void PrintObserver::create_global(KVALUE* kvalue, KVALUE* initializer) {
  DEBUG_STDOUT("<<<<< CREATE GLOBAL >>>>> value:" << KVALUE_ToString(kvalue) << ", initializer:" << KVALUE_ToString(initializer));
}

void PrintObserver::create_global_array(int valInx UNUSED, SCOPE scope UNUSED, KIND varType UNUSED, uint64_t addr UNUSED, uint32_t size, KIND type) {
  DEBUG_STDOUT("<<<<< CREATE GLOBAL ARRAY >>>>> value: TOPRINT" << ", size:" << size<< ", type:" << KIND_ToString(type));
}

void PrintObserver::call(IID iid, bool nounwind, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::call_sin(IID iid, bool nounwind, int pc, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL SIN >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", line:" << pc << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::call_acos(IID iid, bool nounwind, int pc, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL ACOS >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", line:" << pc << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::call_sqrt(IID iid, bool nounwind, int pc, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL SQRT >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", line:" << pc << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::call_cos(IID iid, bool nounwind, int pc, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL COS >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", line:" << pc << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::call_log(IID iid, bool nounwind, int pc, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL LOG >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", line:" << pc << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::call_floor(IID iid, bool nounwind, int pc, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL FLOOR >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", line:" << pc << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::call_fabs(IID iid, bool nounwind, int pc, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL FABS >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", line:" << pc << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");
}

void PrintObserver::vaarg() {
  DEBUG_STDOUT("<<<<< VAARG >>>>>\n");
}


void PrintObserver::landingpad() {
  DEBUG_STDOUT("<<<<< LANDINGPAD >>>>>\n");
}

