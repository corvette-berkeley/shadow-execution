
#ifndef INSTRUCTION_OBSERVER_H_
#define INSTRUCTION_OBSERVER_H_

#include "Common.h"
#define UNUSED __attribute__((__unused__))

// interface/adapter for observers
// all methods have default (empty) implementations, so no need to override all
class InstructionObserver {
public:
	InstructionObserver(std::string name) : name_(name) {}
	virtual ~InstructionObserver() {}

	// ***** Binary Operators **** //
	virtual void add(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void fadd(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void sub(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void fsub(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void mul(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void fmul(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void udiv(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void sdiv(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void fdiv(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void urem(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void srem(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void frem(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};

	// ***** Bitwise Binary Operators ***** //
	virtual void shl(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void lshr(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void ashr(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void and_(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void or_(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void xor_(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};

	// ***** Vector Operations ***** //
	virtual void extractelement(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void insertelement() {};
	virtual void shufflevector() {};

	// ***** Aggregate Operations ***** //
	virtual void extractvalue(IID iid UNUSED, KVALUE* op UNUSED) {};
	virtual void insertvalue(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};

	// ***** Memory Access and Addressing Operations ***** //
	virtual void allocax(IID iid UNUSED, KIND kind UNUSED) {};
	virtual void load(IID iid UNUSED, PTR addr UNUSED, IID addr_iid UNUSED, KVALUE* value UNUSED) {};
	virtual void store(IID iid UNUSED, PTR addr UNUSED, IID addr_iid UNUSED, KVALUE* value UNUSED) {};
	virtual void fence() {};
	virtual void cmpxchg(IID iid UNUSED, PTR addr UNUSED, KVALUE* value1 UNUSED, KVALUE* value2 UNUSED) {};
	virtual void atomicrmw() {};
	virtual void getelementptr(IID iid UNUSED, bool inbound UNUSED, KVALUE* value UNUSED) {};

	// ***** Conversion Operations ***** //
	virtual void trunc(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void zext(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void sext(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void fptrunc(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void fpext(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void fptoui(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void fptosi(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void uitofp(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void sitofp(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void ptrtoint(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void inttoptr(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};
	virtual void bitcast(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED) {};

	// ***** Terminator Instructions *****/
	virtual void branch(IID iid UNUSED, bool conditional UNUSED,  KVALUE* op1 UNUSED) {};
	virtual void branch2(IID iid UNUSED, bool conditional UNUSED) {};
	virtual void indirectbr(IID iid UNUSED, KVALUE* op1 UNUSED) {};
	virtual void invoke(IID iid UNUSED, KVALUE* op UNUSED) {};
	virtual void resume(IID iid UNUSED, KVALUE* op1 UNUSED) {};
	virtual void return_(IID iid UNUSED, KVALUE* op1 UNUSED) {};
	virtual void return2_(IID iid UNUSED) {};
	virtual void switch_(IID iid UNUSED, KVALUE* op UNUSED) {};
	virtual void unreachable() {};

	// ***** Other Operations ***** //
	virtual void icmp(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, PRED pred UNUSED) {};
	virtual void fcmp(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, PRED pred UNUSED) {};
	virtual void phinode() {};
	virtual void select(IID iid UNUSED, KVALUE* cond UNUSED, KVALUE* tvalue UNUSED, KVALUE* fvealue UNUSED) {};
	virtual void push_stack(KVALUE* value UNUSED) {};
	virtual void call(IID iid UNUSED, KVALUE* value UNUSED) {};
	virtual void vaarg() {};
	virtual void landingpad() {};

private:
	std::string name_;
};


#endif /* INSTRUCTIONOBSERVER_H_ */
