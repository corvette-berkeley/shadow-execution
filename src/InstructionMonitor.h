
#ifndef INSTRUCTION_MONITOR_H_
#define INSTRUCTION_MONITOR_H_

#include "Common.h"
#include "InstructionObserver.h"


/*******************************************************************************************/

extern "C" {
  //void llvm_load(IID iid, PTR addr, KVALUE* value);
  void llvm_store(IID iid, PTR addr, KVALUE* value);

  // ***** Binary Operations ***** //
  void llvm_add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  void llvm_fadd();
  void llvm_sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2);
  void llvm_fsub();
  void llvm_mul();
  void llvm_fmul();
  void llvm_udiv();
  void llvm_sdiv();
  void llvm_fdiv();
  void llvm_urem();
  void llvm_srem();
  void llvm_frem();

  // ***** Bitwise Binary Operations ***** //
  void llvm_shl();
  void llvm_lshr();
  void llvm_ashr();
  void llvm_and_();
  void llvm_or_();
  void llvm_xor_();

  void llvm_alloca();
  void llvm_call();

  // ***** CastInst ***** //
  void llvm_bitcast();
  void llvm_fpext();
  void llvm_fptosi();
  void llvm_fptoui();
  void llvm_fptrunc();

  void llvm_load();

  // ***** CmpInst ***** //
  void llvm_fcmp();
  void llvm_icmp();

  void llvm_extract_element();
  void llvm_fence();
  void llvm_get_element_ptr();
  void llvm_insert_element();
  void llvm_insert_value();
  void llvm_landing_pad();
  void llvm_phinode();
  void llvm_select();
  void llvm_shuffle_vector();

  // ***** TerminatorCast ***** //
  void llvm_branch();
  void llvm_indirectbr();
  void llvm_invoke();
  void llvm_resume();
  void llvm_return_();
  void llvm_switch_();
  void llvm_unreachable();

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
