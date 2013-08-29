
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
	virtual void fadd() {};
	virtual void sub(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void fsub() {};
	virtual void mul() {};
	virtual void fmul() {};
	virtual void udiv() {};
	virtual void sdiv() {};
	virtual void fdiv() {};
	virtual void urem() {};
	virtual void srem() {};
	virtual void frem() {};

	// ***** Bitwise Binary Operators ***** //
	virtual void shl() {};
	virtual void lshr() {};
	virtual void ashr() {};
	virtual void and_() {};
	virtual void or_() {};
	virtual void xor_() {};


	virtual void call() {};

	// ***** CastInst ***** //
	virtual void bitcast() {};
	virtual void fpext() {};
	virtual void fptosi() {};
	virtual void fptoui() {};
	virtual void fptrunc() {};

	// ***** CmpInst ***** //
	virtual void fcmp() {};
	virtual void icmp() {};

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


	virtual void landing_pad() {};
	virtual void phinode() {};
	virtual void select() {};

	// ***** TerminatorInst *****/
	virtual void branch() {};
	virtual void indirectbr() {};
	virtual void invoke() {};
	virtual void resume() {};
	virtual void return_() {};
	virtual void switch_() {};
	virtual void unreachable() {};


private:
	std::string name_;
};


#endif /* INSTRUCTIONOBSERVER_H_ */
