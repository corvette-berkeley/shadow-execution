/**
 * @file InterpreterObserver.h
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

#ifndef INTERPRETER_OBSERVER_H_
#define INTERPRETER_OBSERVER_H_

#include "InstructionObserver.h"
#include "IValue.h"
#include <stack>
#include <queue>
#include <vector>
#include <map>

namespace llvm {
  class CmpInst;
}

using namespace std;
using namespace llvm;

class InterpreterObserver : public InstructionObserver {

 protected:
  typedef uint64_t IID;

  stack< vector< IValue* > > executionStack;
  vector< IValue* > globalSymbolTable;

  stack<KVALUE*> myStack; // store arguments of call instruction
  queue<uint64_t> getElementPtrIndexList; // store indices of getelementptr instruction
  queue<uint64_t> arraySize; // store size of array
  queue<KIND> structType; // store struct type
  queue<uint64_t> structElementSize; // store struct (non-flatten) element size
  queue<KVALUE*> returnStruct; // store values of returned struct

  stack<int> callerVarIndex; // index of callee register; to be assigned to the value of call return
  stack<IValue*> callArgs; // copy value from callers to callee arguments
  map<int, KVALUE*> phinodeConstantValues; // store phinode value pairs for constants
  map<int, int> phinodeValues; // store phinode value pairs for values

  stack<int> recentBlock; // record the most recent block visited

  bool isReturn; // whether return instruction is just executed

  long double getValueFromConstant(KVALUE* op); 

  long double getValueFromIValue(IValue* loc); 

  bool debug; // whether to print debugging information

  std::string BINOP_ToString(int binop); 

  std::string BITWISE_ToString(int bitwise);

  void binop(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx, BINOP op);

  void bitwise(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx, BITWISE op);

 public:

  InterpreterObserver(std::string name) : InstructionObserver(name) {
    debug =  true;
    isReturn = false;
  }
  
  virtual void load(IID iid, KIND kind, KVALUE* op, int file, int line, int inx);
  
  virtual void load_struct(IID iid, KIND kind, KVALUE* op, int file, int line, int inx);

  // ***** Binary Operations ***** //
  
  virtual void add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  // ***** Bitwise Binary Operations ***** //
  virtual void shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx);
  
  // ***** Vector Operations ***** //
  virtual void extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx);
  
  virtual void insertelement();
  
  virtual void shufflevector();
  
  // ***** AGGREGATE OPERATIONS ***** //
  virtual void extractvalue(IID iid, int inx, int opinx);
  
  virtual void insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx);
  
  // ***** Memory Access and Addressing Operations ***** //
  virtual void allocax(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, KVALUE* result);

  virtual void allocax_array(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, KVALUE* addr);

  virtual void allocax_struct(IID iid, uint64_t size, int inx, int line, bool arg, KVALUE* addr);
  
  virtual void store(IID iid, KVALUE* op, KVALUE* kv, int file, int line, int inx);
  
  virtual void fence();
  
  virtual void cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx);

  virtual void atomicrmw();

  virtual void getelementptr(IID iid, bool inbound, KVALUE* op, KVALUE* index, KIND kind, uint64_t size,int line, int inx);

  virtual void getelementptr_array(IID iid, bool inbound, KVALUE* op, KIND kind, int elementSize, int inx);

  virtual void getelementptr_struct(IID iid, bool inbound, KVALUE* op, KIND kind, KIND arrayKind, int inx);

  // ***** Conversion Operations ***** //
  virtual void trunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void zext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void sext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void fptrunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void fpext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void fptoui(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void fptosi(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void uitofp(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void sitofp(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void ptrtoint(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void inttoptr(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  virtual void bitcast(IID iid, KIND type, KVALUE* op, uint64_t size, int inx);
  
  // ***** TerminatorInst ***** //
  virtual void branch(IID iid, bool conditional, KVALUE* op1, int inx);
  
  virtual void branch2(IID iid, bool conditional, int inx);
  
  virtual void indirectbr(IID iid, KVALUE* op1, int inx);
  
  virtual void invoke(IID iid, KVALUE* call_value, int inx);
  
  virtual void resume(IID iid, KVALUE* op1, int inx);
  
  virtual void return_(IID iid, KVALUE* op1, int inx);
  
  virtual void return2_(IID iid, int inx);
  
  virtual void return_struct_(IID iid, int inx, int valInx);

  virtual void switch_(IID iid, KVALUE* op, int inx);
  
  virtual void unreachable();
  
  // ***** Other Operations ***** //
  virtual void icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx);
  
  virtual void fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx);
  
  virtual void phinode(IID iid, int inx);
  
  virtual void select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx);
  
  virtual void call(IID iid, bool nounwind, KIND type, int inx);

  virtual void call_malloc(IID iid, bool nounwind, KIND type, KVALUE* call_value, int size, int inx, KVALUE* mallocAddress);
  
  virtual void vaarg();
  
  virtual void landingpad();
  
  void push_stack(KVALUE* value);

  void push_return_struct(KVALUE* value);

  void push_phinode_constant_value(KVALUE* value, int blockId);

  void push_phinode_value(int valId, int blockId);

  void push_struct_type(KIND kind);

  void push_struct_element_size(uint64_t s);

  void push_getelementptr_inx(KVALUE* int_value);

  void push_getelementptr_inx2(int int_value);

  void push_array_size(uint64_t size);

  void after_call(KVALUE* value);

  void after_void_call();

  void after_struct_call();

  void create_stack_frame(int size);
  
  void create_global_symbol_table(int size);

  void record_block_id(int id);

  void create_global(KVALUE* value, KVALUE* initializer);

  void printCurrentFrame();

  bool syncLoad(IValue* iValue, KVALUE* concrete, KIND type);

  bool checkStore(IValue *dest, KVALUE *kv);

  /**
   * Find the index for the IValue array object, given the offset.
   *
   * @note the function uses binary search to search for the index such that
   * the IValue object at this index has the largest firstByte value that is
   * less than or equal to the offset.
   *
   * @param values the array IValue object 
   * @param offset the distance to the base pointer of the IValue object
   * @param length length of the array IValue object
   *
   * @return the index corresponding to the offset
   */
  unsigned findIndex(IValue* values, unsigned offset, unsigned length);

};


#endif /* INTERPRETER_OBSERVER_H_ */
