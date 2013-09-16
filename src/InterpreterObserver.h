#ifndef INTERPRETER_OBSERVER_H_
#define INTERPRETER_OBSERVER_H_

#include "InstructionObserver.h"
#include "Location.h"
#include <stack>

using namespace std;

/*******************************************************************************************/
// helpful macros for defining instrumenters
#define DEFAULT_CONSTRUCTOR(T) \
		T(std::string name) \
		: InstructionObserver(name) {}

/*******************************************************************************************/

class InterpreterObserver : public InstructionObserver {

  typedef uint64_t IID;
  typedef map<IID, Location*> Frame;

 private:
  stack<KVALUE*> myStack;
  Frame* currentFrame;
  stack<Frame*> executionStack;

 public:
  
  //DEFAULT_CONSTRUCTOR(InterpreterObserver);
 InterpreterObserver(std::string name) : InstructionObserver(name) {
    currentFrame = new Frame();
    executionStack.push(currentFrame);
  }
  
  virtual void load(IID iid, PTR addr, IID addr_iid, KVALUE* kv);
  
  // ***** Binary Operations ***** //
  
  virtual void add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  // ***** Bitwise Binary Operations ***** //
  virtual void shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  virtual void xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  
  // ***** Vector Operations ***** //
  virtual void extractelement(IID iid, KVALUE* op1, KVALUE* op2);
  
  virtual void insertelement();
  
  virtual void shufflevector();
  
  // ***** AGGREGATE OPERATIONS ***** //
  virtual void extractvalue(IID iid, KVALUE* op);
  
  virtual void insertvalue(IID iid, KVALUE* op1, KVALUE* op2);
  
  // ***** Memory Access and Addressing Operations ***** //
  virtual void allocax(IID iid, KIND kind);
  
  virtual void store(IID iid, PTR addr, IID addr_iid, KVALUE* kv);
  
  virtual void fence();
  
  virtual void cmpxchg();
  
  virtual void cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2);

  virtual void atomicrmw();

  virtual void getelementptr(IID iid, bool inbound, KVALUE* op);

  // ***** Conversion Operations ***** //
  virtual void trunc(IID iid, KVALUE* op);
  
  virtual void zext(IID iid, KVALUE* op);
  
  virtual void sext(IID iid, KVALUE* op);
  
  virtual void fptrunc(IID iid, KVALUE* op);
  
  virtual void fpext(IID iid, KVALUE* op);
  
  virtual void fptoui(IID iid, KVALUE* op);
  
  virtual void fptosi(IID iid, KVALUE* op);
  
  virtual void uitofp(IID iid, KVALUE* op);
  
  virtual void sitofp(IID iid, KVALUE* op);
  
  virtual void ptrtoint(IID iid, KVALUE* op);
  
  virtual void inttoptr(IID iid, KVALUE* op);
  
  virtual void bitcast(IID iid, KVALUE* op);
  
  // ***** TerminatorInst ***** //
  virtual void branch(IID iid, bool conditional, KVALUE* op1);
  
  virtual void branch2(IID iid, bool conditional);
  
  virtual void indirectbr(IID iid, KVALUE* op1);
  
  virtual void invoke(IID iid, KVALUE* call_value);
  
  virtual void resume(IID iid, KVALUE* op1);
  
  virtual void return_(IID iid, KVALUE* op1);
  
  virtual void return2_(IID iid);
  
  virtual void switch_(IID iid, KVALUE* op);
  
  virtual void unreachable();
  
  // ***** Other Operations ***** //
  virtual void icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred);
  
  virtual void fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred);
  
  virtual void phinode();
  
  virtual void select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue);
  
  virtual void select();
  
  virtual void call(IID iid, KVALUE* call_value);
  
  virtual void vaarg();
  
  virtual void landingpad();
  
  void push_stack(KVALUE* value);

  void printCurrentFrame();

};


#endif /* INTERPRETER_OBSERVER_H_ */
