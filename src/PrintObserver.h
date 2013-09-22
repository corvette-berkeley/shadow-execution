#ifndef PRINT_OBSERVER_H_
#define PRINT_OBSERVER_H_

#include "InstructionObserver.h"
#include <stack>

using namespace std;

/*******************************************************************************************/
// helpful macros for defining instrumenters
#define DEFAULT_CONSTRUCTOR(T) \
		T(std::string name) \
		: InstructionObserver(name) {}

/*******************************************************************************************/

class PrintObserver : public InstructionObserver {
public:
  stack<KVALUE*> myStack;
	DEFAULT_CONSTRUCTOR(PrintObserver);
	
	virtual void load(IID iid, PTR addr, IID addr_iid, KVALUE* kv, int inx);

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
	virtual void extractvalue(IID iid, KVALUE* op, int inx);

	virtual void insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx);

	// ***** Memory Access and Addressing Operations ***** //
	virtual void allocax(IID iid, KIND kind, int inx);

	virtual void store(IID iid, PTR addr, IID addr_iid, KVALUE* kv, int inx);

	virtual void fence();

	virtual void cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx);

	virtual void atomicrmw();

	virtual void getelementptr(IID iid, bool inbound, KVALUE* op, int inx);

	// ***** Conversion Operations ***** //
	virtual void trunc(IID iid, KIND type, KVALUE* op, int inx);

	virtual void zext(IID iid, KIND type, KVALUE* op, int inx);

	virtual void sext(IID iid, KIND type, KVALUE* op, int inx);

	virtual void fptrunc(IID iid, KIND type, KVALUE* op, int inx);

	virtual void fpext(IID iid, KIND type, KVALUE* op, int inx);

	virtual void fptoui(IID iid, KIND type, KVALUE* op, int inx);

	virtual void fptosi(IID iid, KIND type, KVALUE* op, int inx);

	virtual void uitofp(IID iid, KIND type, KVALUE* op, int inx);

	virtual void sitofp(IID iid, KIND type, KVALUE* op, int inx);

	virtual void ptrtoint(IID iid, KIND type, KVALUE* op, int inx);

	virtual void inttoptr(IID iid, KIND type, KVALUE* op, int inx);

	virtual void bitcast(IID iid, KIND type, KVALUE* op, int inx);


	// ***** TerminatorInst ***** //
	virtual void branch(IID iid, bool conditional, KVALUE* op1, int inx);

	virtual void branch2(IID iid, bool conditional, int inx);

	virtual void indirectbr(IID iid, KVALUE* op1, int inx);

	virtual void invoke(IID iid, KVALUE* call_value, int inx);

	virtual void resume(IID iid, KVALUE* op1, int inx);

	virtual void return_(IID iid, KVALUE* op1, int inx);

	virtual void return2_(IID iid, int inx);

	virtual void switch_(IID iid, KVALUE* op, int inx);

	virtual void unreachable();

	// ***** Other Operations ***** //
	virtual void icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx);

	virtual void fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx);
	
	virtual void phinode();

	virtual void select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx);

	virtual void call(IID iid, KVALUE* call_value, int inx);

	virtual void vaarg();

	virtual void landingpad();

	void push_stack(KVALUE* value);
};


#endif /* PRINT_OBSERVER_H_ */
