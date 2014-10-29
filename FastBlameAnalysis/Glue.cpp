#include <unordered_map>
#include "Glue.h"
#include "BlameAnalysis.h"

using std::unordered_map;

unordered_map<IID, IID> fake_to_real_iid;
unordered_map<unsigned, IID> arg_to_real_iid;
IID return_iid;

IID translate_to_real(IID val) {
	if (fake_to_real_iid.find(val) == fake_to_real_iid.end()) {
		return val;
	}
	return fake_to_real_iid[val];
}

void llvm_fadd(IID iidf, double, IID l, double lo, IID r, double ro) {
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fadd(iidf, l, r, lo, ro);
}

void llvm_fsub(IID iidf, double, IID l, double lo, IID r, double ro) {
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fsub(iidf, l, r, lo, ro);
}

void llvm_fmul(IID iidf, double, IID l, double lo, IID r, double ro) {
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fmul(iidf, l, r, lo, ro);
}

void llvm_fdiv(IID iidf, double, IID l, double lo, IID r, double ro) {
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fdiv(iidf, l, r, lo, ro);
}

void llvm_frem(IID, double, IID, double, IID, double) {
	// TODO: We did not implement frem - this may be necessary
	//	BlameAnalysis::get().frem(iidf, l, r, lo, ro);
}

unordered_map<void*, IID> ptr_to_iid;
void llvm_fload(IID iidf, double, IID, void* vptr) {
	fake_to_real_iid[iidf] = ptr_to_iid[vptr];
	// BlameAnalysis::get().load(iidf, input, value);
}

void llvm_fstore(IID iidV, double, IID, void* vptr) {
	if (iidV >= 0) {
		ptr_to_iid[vptr] = iidV;
	} else {
		assert(arg_to_real_iid.find(-iidV) != arg_to_real_iid.end());
		ptr_to_iid[vptr] = fake_to_real_iid[arg_to_real_iid[-iidV]];
	}
	//	BlameAnalysis::get().store(iidV, ptr, value);
}

void llvm_fphi(IID out, double, IID in) {
	fake_to_real_iid[out] = in;
}

// ***** Other Operations ***** //
void llvm_call_fabs(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_fabs(iidf, operand, operandValue);
}

void llvm_call_exp(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_exp(iidf, operand, operandValue);
}
void llvm_call_sqrt(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_sqrt(iidf, operand, operandValue);
}
void llvm_call_log(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_log(iidf, operand, operandValue);
}
void llvm_call_sin(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_sin(iidf, operand, operandValue);
}
void llvm_call_acos(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_acos(iidf, operand, operandValue);
}
void llvm_call_cos(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_cos(iidf, operand, operandValue);
}
void llvm_call_floor(IID iidf, double, IID operand, double operandValue) {
	operand = translate_to_real(operand);
	BlameAnalysis::get().call_floor(iidf, operand, operandValue);
}

void llvm_arg(unsigned argInx, IID iid) {
	arg_to_real_iid[argInx] = iid;
}

void llvm_return(IID iid) {
	return_iid = iid;
}

void llvm_after_call(IID iid) {
	fake_to_real_iid[iid] = fake_to_real_iid[return_iid];
}
