#include <unordered_map>
#include <iostream>
#include "Glue.h"
#include "BlameAnalysis.h"

using std::unordered_map;
using std::cout;
using std::endl;

unordered_map<unsigned, IID> arg_to_real_iid;
unordered_map<void*, IID> ptr_to_iid;
IID return_iid;

void llvm_fadd(IID iidf, double v, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fadd(iidf, l, r, v, lo, ro);
}

void llvm_fsub(IID iidf, double v, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fsub(iidf, l, r, v, lo, ro);
}

void llvm_fmul(IID iidf, double v, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fmul(iidf, l, r, v, lo, ro);
}

void llvm_fdiv(IID iidf, double v, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fdiv(iidf, l, r, v, lo, ro);
}

void llvm_frem(IID, double, IID, double, IID, double) {
	// TODO: We did not implement frem - this may be necessary
	//	BlameAnalysis::get().frem(iidf, l, r, lo, ro);
}

void llvm_oeq(IID iidf, bool, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().oeq(iidf, l, r, lo, ro);
}

void llvm_ogt(IID iidf, bool, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().ogt(iidf, l, r, lo, ro);
}

void llvm_oge(IID iidf, bool, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().oge(iidf, l, r, lo, ro);
}

void llvm_olt(IID iidf, bool, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().olt(iidf, l, r, lo, ro);
}

void llvm_ole(IID iidf, bool, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().ole(iidf, l, r, lo, ro);
}

void llvm_one(IID iidf, bool, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().one(iidf, l, r, lo, ro);
}

void llvm_fload(IID iidV, double v, IID iid, void* vptr) {
	if (!BlameAnalysis::get().startTrack(iid)) {
		return;
	}

	if (ptr_to_iid.find(vptr) == ptr_to_iid.end()) {
		iid = -1;
	} else {
		iid = ptr_to_iid[vptr];
	}
	BlameAnalysis::get().fload(iidV, v, iid, vptr);
}

void llvm_fstore(IID iidV, double, IID iid, void* vptr) {
	if (!BlameAnalysis::get().startTrack(iid)) {
		return;
	}

	if (iidV < 0) {
		assert(arg_to_real_iid.find(-iidV) != arg_to_real_iid.end());
		iidV = arg_to_real_iid[-iidV];
	}

	ptr_to_iid[vptr] = iidV;
}

void llvm_fphi(IID out, double v, IID in) {
	BlameAnalysis::get().fphi(out, v, in);
}

// ***** Other Operations ***** //
void llvm_call_fabs(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_fabs(iidf, v, operand, operandValue);
}

void llvm_call_exp(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_exp(iidf, v, operand, operandValue);
}
void llvm_call_sqrt(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_sqrt(iidf, v, operand, operandValue);
}
void llvm_call_log(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_log(iidf, v, operand, operandValue);
}
void llvm_call_sin(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_sin(iidf, v, operand, operandValue);
}
void llvm_call_acos(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_acos(iidf, v, operand, operandValue);
}
void llvm_call_cos(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_cos(iidf, v, operand, operandValue);
}
void llvm_call_floor(IID iidf, double v, IID operand, double operandValue) {
	BlameAnalysis::get().call_floor(iidf, v, operand, operandValue);
}
void llvm_call_pow(IID iidf, double, IID operand01, double operandValue01, IID operand02, double operandValue02) {
	BlameAnalysis::get().call_pow(iidf, operand01, operandValue01, operand02, operandValue02);
}

void llvm_arg(unsigned argInx, IID iid) {
	arg_to_real_iid[argInx] = iid;
}

void llvm_return(IID iid) {
	return_iid = iid;
}

void llvm_after_call(IID iid, double v) {
	BlameAnalysis::get().fafter_call(iid, v, return_iid);
	return_iid = -1;  // invalidate this return id
}
