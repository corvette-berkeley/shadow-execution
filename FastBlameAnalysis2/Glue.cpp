#include <unordered_map>
#include <iostream>
#include "Glue.h"
#include "BlameAnalysis.h"

using std::unordered_map;
using std::cout;
using std::endl;

unordered_map<unsigned, IID> arg_to_real_iid;
unordered_map<void*, IID> ptr_to_iid;
unordered_map<IID, void*> iid_to_ptr;
IID return_iid;

void* translate_to_ptr(IID val) {
	if (iid_to_ptr.find(val) != iid_to_ptr.end()) {
		return iid_to_ptr[val];
	}

	return 0;
}

void llvm_fadd(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	BlameAnalysis::get().fadd(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_fsub(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	BlameAnalysis::get().fsub(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_fmul(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	BlameAnalysis::get().fmul(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_fdiv(IID iidf, double, IID l, double lo, IID r, double ro) {
	void* lptr = translate_to_ptr(l);
	void* rptr = translate_to_ptr(r);
	BlameAnalysis::get().fdiv(iidf, l, r, lptr, rptr, lo, ro);
}

void llvm_frem(IID, double, IID, double, IID, double) {
	// TODO: We did not implement frem - this may be necessary
	//	BlameAnalysis::get().frem(iidf, l, r, lo, ro);
}

void llvm_fload(IID iidV, double v, IID iid, void* vptr) {
	if (iidV == 84) {
		cout << "LOAD" << endl;
		cout << "IIDV: " << iidV << endl;
		cout << v << endl;
		cout << "---" << endl;
	}
	iid = ptr_to_iid[vptr];
	iid_to_ptr[iidV] = vptr;
	BlameAnalysis::get().fload(iidV, iid, vptr);
}

void llvm_fstore(IID iidV, double, IID, void* vptr) {
	if (iidV < 0) {
		assert(arg_to_real_iid.find(-iidV) != arg_to_real_iid.end());
		iidV = arg_to_real_iid[-iidV];
	}

	ptr_to_iid[vptr] = iidV;
	BlameAnalysis::get().fstore(iidV, vptr);
}

void llvm_fphi(IID out, double, IID in) {
	BlameAnalysis::get().fphi(out, in);
}

// ***** Other Operations ***** //
void llvm_call_fabs(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_fabs(iidf, operand, ptr, operandValue);
}

void llvm_call_exp(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_exp(iidf, operand, ptr, operandValue);
}
void llvm_call_sqrt(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_sqrt(iidf, operand, ptr, operandValue);
}
void llvm_call_log(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_log(iidf, operand, ptr, operandValue);
}
void llvm_call_sin(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_sin(iidf, operand, ptr, operandValue);
}
void llvm_call_acos(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_acos(iidf, operand, ptr, operandValue);
}
void llvm_call_cos(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_cos(iidf, operand, ptr, operandValue);
}
void llvm_call_floor(IID iidf, double, IID operand, double operandValue) {
	void* ptr = translate_to_ptr(operand);
	BlameAnalysis::get().call_floor(iidf, operand, ptr, operandValue);
}

void llvm_arg(unsigned argInx, IID iid) {
	arg_to_real_iid[argInx] = iid;
}

void llvm_return(IID iid) {
	return_iid = iid;
}

void llvm_after_call(IID iid) {
	BlameAnalysis::get().fafter_call(iid, return_iid);
	return_iid = -1;  // invalidate this return id
}
