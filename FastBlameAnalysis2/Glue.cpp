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

void llvm_fadd(IID iidf, double, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fadd(iidf, l, r, lo, ro);
}

void llvm_fsub(IID iidf, double, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fsub(iidf, l, r, lo, ro);
}

void llvm_fmul(IID iidf, double, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fmul(iidf, l, r, lo, ro);
}

void llvm_fdiv(IID iidf, double, IID l, double lo, IID r, double ro) {
	BlameAnalysis::get().fdiv(iidf, l, r, lo, ro);
}

void llvm_frem(IID, double, IID, double, IID, double) {
	// TODO: We did not implement frem - this may be necessary
	//	BlameAnalysis::get().frem(iidf, l, r, lo, ro);
}

void llvm_fload(IID iidV, double v, IID iid, void* vptr) {
	/*
	if (iidV == 3499) {
	  cout << "LOAD" << endl;
	  cout << "IIDV: " << iidV << endl;
	  cout << "IID: " << iid << endl;
	  cout << "PTR: " << vptr << endl;
	  cout << "REAL IID: " << ptr_to_iid[vptr] << endl;
	  cout << "VALUE: " << v << endl;
	  cout << "-----" << endl;
	}
	*/
	iid = ptr_to_iid[vptr];
	BlameAnalysis::get().fload(iidV, v, iid, vptr);
}

void llvm_fstore(IID iidV, double, IID, void* vptr) {
	/*
	if (iidV == 3275) {
	  cout << "STORE" << endl;
	  cout << "IIDV: " << iidV << endl;
	  cout << "PTR: " << vptr << endl;
	  cout << "VALUE: " << v << endl;
	  cout << "----" << endl;
	}
	*/
	if (iidV < 0) {
		assert(arg_to_real_iid.find(-iidV) != arg_to_real_iid.end());
		iidV = arg_to_real_iid[-iidV];
	}

	ptr_to_iid[vptr] = iidV;
	BlameAnalysis::get().fstore(iidV, vptr);
}

void llvm_fphi(IID out, double v, IID in) {
	BlameAnalysis::get().fphi(out, v, in);
}

// ***** Other Operations ***** //
void llvm_call_fabs(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_fabs(iidf, operand, operandValue);
}

void llvm_call_exp(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_exp(iidf, operand, operandValue);
}
void llvm_call_sqrt(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_sqrt(iidf, operand, operandValue);
}
void llvm_call_log(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_log(iidf, operand, operandValue);
}
void llvm_call_sin(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_sin(iidf, operand, operandValue);
}
void llvm_call_acos(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_acos(iidf, operand, operandValue);
}
void llvm_call_cos(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_cos(iidf, operand, operandValue);
}
void llvm_call_floor(IID iidf, double, IID operand, double operandValue) {
	BlameAnalysis::get().call_floor(iidf, operand, operandValue);
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
