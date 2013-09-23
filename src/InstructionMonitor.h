
#ifndef INSTRUCTION_MONITOR_H_
#define INSTRUCTION_MONITOR_H_

#include "Common.h"
#include "InstructionObserver.h"


/*******************************************************************************************/

extern "C" {

  // ***** Binary Operations ***** //
  void llvm_add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);

  // ***** Bitwise Binary Operations ***** //
  void llvm_shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);
  void llvm_xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int x);

  // ***** Vector Operations ***** //
  void llvm_extractelement(IID iid, KVALUE* op1, KVALUE* op2, int x);
  void llvm_insertelement();
  void llvm_shufflevector();

  // ***** Aggregate Operations ***** //
  void llvm_extractvalue(IID iid, KVALUE* op, int x);
  void llvm_insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int x);

  // ***** Memory Access and Addressing Operations ***** //
  void llvm_allocax(IID iid, KIND kind, int x);
  void llvm_load(IID iid, KVALUE* op, KVALUE* value, int x);
  void llvm_store(IID iid, KVALUE* op, KVALUE* value, int x);
  void llvm_fence();
  void llvm_cmpxchg(IID iid, PTR addr, KVALUE* value1, KVALUE* value2, int x);
  void llvm_atomicrmw();
  void llvm_getelementptr(IID iid, bool inbound, KVALUE* value, int x);

  // ***** Conversion Operations ***** //
  void llvm_trunc(IID iid, KIND type, KVALUE* op, int x);
  void llvm_zext(IID iid, KIND type, KVALUE* op, int x);
  void llvm_sext(IID iid, KIND type, KVALUE* op, int x);
  void llvm_fptrunc(IID iid, KIND type, KVALUE* op, int x);
  void llvm_fpext(IID iid, KIND type, KVALUE* op, int x);
  void llvm_fptoui(IID iid, KIND type, KVALUE* op, int x);
  void llvm_fptosi(IID iid, KIND type, KVALUE* op, int x);
  void llvm_uitofp(IID iid, KIND type, KVALUE* op, int x);
  void llvm_sitofp(IID iid, KIND type, KVALUE* op, int x);
  void llvm_ptrtoint(IID iid, KIND type, KVALUE* op, int x);
  void llvm_inttoptr(IID iid, KIND type, KVALUE* op, int x);
  void llvm_bitcast(IID iid, KIND type, KVALUE* op, int x);

  // ***** Terminator Instructions ***** //
  void llvm_branch(IID iid, bool conditional, KVALUE* op1, int x);
  void llvm_branch2(IID iid, bool conditional, int x);
  void llvm_indirectbr(IID iid, KVALUE* op1, int x);
  void llvm_invoke(IID iid, KVALUE op, int x);
  void llvm_resume(IID iid, KVALUE* op1, int x);
  void llvm_return_(IID iid, KVALUE* op1, int x);
  void llvm_return2_(IID iid, int x);
  void llvm_switch_(IID iid, KVALUE* op, int x);
  void llvm_unreachable();

  // ***** Other Operations ***** //
  void llvm_icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int x);
  void llvm_fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int x);
  void llvm_phinode();
  void llvm_select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int x);
  void llvm_push_stack(KVALUE* value);
  void llvm_create_stack_frame(int size);
  void llvm_call(IID iid, KIND type, KVALUE* value, int x);
  void llvm_vaarg();
  void llvm_landingpad();

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
