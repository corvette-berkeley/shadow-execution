
#ifndef INSTRUCTION_MONITOR_H_
#define INSTRUCTION_MONITOR_H_

#include "Common.h"
#include "InstructionObserver.h"


/*******************************************************************************************/

extern "C" {
	void llvm_load(IID iid, PTR addr, KVALUE* value);
	void llvm_store(IID iid, PTR addr, KVALUE* value);

	void llvm_add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
	void llvm_sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
}

/*******************************************************************************************/

typedef std::vector<InstructionObserver*> ObserverPtrList;

extern ObserverPtrList observers_;

template<class T>
class RegisterObserver {
public:
	RegisterObserver(std::string name) {
		fprintf(stderr, ">>> Registering observer: %s\n", name.c_str());

		observers_.push_back(new T(name));
	}
};


#endif // INSTRUCTION_MONITOR_H_
