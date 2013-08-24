
#ifndef INSTRUCTION_OBSERVER_H_
#define INSTRUCTION_OBSERVER_H_

#include "Common.h"

// interface/adapter for observers
// all methods have default (empty) implementations, so no need to override all
class InstructionObserver {
public:
	InstructionObserver(std::string name) : name_(name) {}
	virtual ~InstructionObserver() {}

	virtual void load(IID iid, PTR addr, KVALUE* value) {};
	virtual void store(IID iid, PTR addr, KVALUE* value) {};

	virtual void add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {};
	virtual void sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {};

private:
	std::string name_;
};


#endif /* INSTRUCTIONOBSERVER_H_ */
