
#ifndef INSTRUCTION_MONITOR_H_
#define INSTRUCTION_MONITOR_H_

#include "Common.h"
#include "InstructionObserver.h"


/*******************************************************************************************/

extern "C" {
  //void llvm_load(IID iid, PTR addr, KVALUE* value);
  void llvm_store(IID iid, PTR addr, KVALUE* value);

  void llvm_add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  void llvm_sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);

  void llvm_alloca();
  void llvm_call();

  // ***** CastInst ***** //
  void llvm_bitcast();
  void llvm_fpext();
  void llvm_fptosi();
  void llvm_fptoui();
  void llvm_fptrunc();

  void llvm_load();

  // ***** TerminatorCast ***** //
  void llvm_branch();
  void llvm_indirectbr();
  void llvm_invoke();
  void llvm_resume();
  void llvm_return_();
  void llvm_switch_();
  void llvm_unreachable();

  // ***** CmpInst ***** //
  void llvm_fcmp();
  void llvm_icmp();
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
