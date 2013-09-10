
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
	virtual void extractelement() {};
	virtual void insertelement() {};
	virtual void shufflevector() {};

	// ***** Aggregate Operations ***** //
	virtual void extractvalue() {};
	virtual void insertvalue() {};

	// ***** Memory Access and Addressing Operations ***** //
	virtual void alloca() {};
	virtual void load() {};
	//virtual void load(IID iid UNUSED, PTR addr UNUSED, KVALUE* value UNUSED) {};
	virtual void store(IID iid UNUSED, PTR addr UNUSED, KVALUE* value UNUSED) {};
	virtual void fence() {};
	virtual void cmpxchg() {};
	virtual void atomicrmw() {};
	virtual void getelementptr() {};

	// ***** Conversion Operations ***** //
	virtual void trunc() {};
	virtual void zext() {};
	virtual void sext() {};
	virtual void fptrunc() {};
	virtual void fpext() {};
	virtual void fptoui() {};
	virtual void fptosi() {};
	virtual void uitofp() {};
	virtual void sitofp() {};
	virtual void ptrtoint() {};
	virtual void inttoptr() {};
	virtual void bitcast() {};

	// ***** Terminator Instructions *****/
	virtual void branch(IID iid UNUSED, bool conditional UNUSED,  KVALUE* op1 UNUSED) {};
	virtual void branch2(IID iid UNUSED, bool conditional UNUSED) {};
	virtual void indirectbr(IID iid UNUSED, KVALUE* op1 UNUSED) {};
	virtual void invoke() {};
	virtual void resume() {};
	virtual void return_() {};
	virtual void switch_() {};
	virtual void unreachable() {};

	// ***** Other Operations ***** //
	virtual void icmp() {};
	virtual void fcmp() {};
	virtual void phinode() {};
	virtual void select() {};
	virtual void call() {};
	virtual void vaarg() {};
	virtual void landingpad() {};

private:
	std::string name_;
};


#endif /* INSTRUCTIONOBSERVER_H_ */
