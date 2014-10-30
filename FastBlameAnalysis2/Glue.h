#include "BlameUtilities.h"

extern "C" {

	// ***** Binary Operations ***** //
	void llvm_fadd(IID iidf, double output, IID l, double lo, IID r, double ro);
	void llvm_fsub(IID iidf, double output, IID l, double lo, IID r, double ro);
	void llvm_fmul(IID iidf, double output, IID l, double lo, IID r, double ro);
	void llvm_fdiv(IID iidf, double output, IID l, double lo, IID r, double ro);
	void llvm_frem(IID iidf, double output, IID l, double lo, IID r, double ro);

	void llvm_fload(IID iidf, double output, IID input, void* input_ptr);
	void llvm_fstore(IID iidV, double value, IID ptr, void* location);

	void llvm_fphi(IID iidV, double value, IID orig);

	// ***** Math Call Operations ***** //
	void llvm_call_fabs(IID iidf, double output, IID operand, double operandValue);
	void llvm_call_exp(IID iidf, double output, IID operand, double operandValue);
	void llvm_call_sqrt(IID iidf, double output, IID operand, double operandValue);
	void llvm_call_log(IID iidf, double output, IID operand, double operandValue);
	void llvm_call_sin(IID iidf, double output, IID operand, double operandValue);
	void llvm_call_acos(IID iidf, double output, IID operand, double operandValue);
	void llvm_call_cos(IID iidf, double output, IID operand, double operandValue);
	void llvm_call_floor(IID iidf, double output, IID operand, double operandValue);

	// ***** Other Operations ***** //
	void llvm_arg(unsigned argInx, IID iid);
	void llvm_return(IID iid);
	void llvm_after_call(IID iid);
}
