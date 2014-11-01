#include <unordered_map>
#include <iostream>
#include "Glue.h"
#include "BlameAnalysis.h"

using std::unordered_map;

unordered_map<IID, IID> fake_to_real_iid;
unordered_map<unsigned, IID> arg_to_real_iid;
unordered_map<void*, IID> ptr_to_iid;
unordered_map<IID, void*> iid_to_ptr;
IID return_iid;

IID translate_to_real(IID val) {
	if (fake_to_real_iid.find(val) == fake_to_real_iid.end()) {
		return val;
	}
	int real_iid = fake_to_real_iid[val];
	return real_iid;
}

void* translate_to_ptr(IID val) {
	if (iid_to_ptr.find(val) != iid_to_ptr.end()) {
		return iid_to_ptr[val];
	}

	return 0;
}

void llvm_fadd(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fadd(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_fsub(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fsub(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_fmul(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fmul(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_fdiv(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	l = translate_to_real(l);
	r = translate_to_real(r);
	BlameAnalysis::get().fdiv(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_frem(IID, double, IID, double, IID, double) {
	// TODO: We did not implement frem - this may be necessary
	//	BlameAnalysis::get().frem(iidf, l, r, lo, ro);
}

void llvm_fload(IID iidf, double, IID, void* vptr) {
	fake_to_real_iid[iidf] = ptr_to_iid[vptr];
	iid_to_ptr[iidf] = vptr;
}

void llvm_fstore(IID iidV, double, IID, void* vptr) {
	if (iidV >= 0) {
		ptr_to_iid[vptr] = iidV;
		if (BlameAnalysis::get().trace.find(iidV) !=
				BlameAnalysis::get().trace.end()) {
			BlameAnalysis::get().trace[iidV][vptr] =
				BlameAnalysis::get().trace[iidV][0];
		}
	} else {
		assert(arg_to_real_iid.find(-iidV) != arg_to_real_iid.end());
		ptr_to_iid[vptr] = fake_to_real_iid[arg_to_real_iid[-iidV]];
	}
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
