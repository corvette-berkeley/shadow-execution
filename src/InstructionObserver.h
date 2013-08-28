
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

	//virtual void load(IID iid UNUSED, PTR addr UNUSED, KVALUE* value UNUSED) {};
	virtual void store(IID iid UNUSED, PTR addr UNUSED, KVALUE* value UNUSED) {};

	virtual void add(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};
	virtual void sub(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED) {};

	virtual void alloca() {};
	virtual void call() {};

	// ***** CastInst ***** //
	virtual void bitcast() {};
	virtual void fpext() {};
	virtual void fptosi() {};
	virtual void fptoui() {};
	virtual void fptrunc() {};

	virtual void load() {};

	// ***** CmpInst ***** //
	virtual void fcmp() {};
	virtual void icmp() {};

	virtual void extract_element() {};
	virtual void fence() {};
	virtual void get_element_ptr() {};
	virtual void insert_element() {};
	virtual void insert_value() {};
	virtual void landing_pad() {};
	virtual void phinode() {};
	virtual void select() {};
	virtual void shuffle_vector() {};

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
