
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
	virtual void return_() {};

private:
	std::string name_;
};


#endif /* INSTRUCTIONOBSERVER_H_ */
