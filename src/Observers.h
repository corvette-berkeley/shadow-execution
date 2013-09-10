#ifndef OBSERVERS_H_
#define OBSERVERS_H_

#include "InstructionObserver.h"

/*******************************************************************************************/
// helpful macros for defining instrumenters
#define DEFAULT_CONSTRUCTOR(T) \
		T(std::string name) \
		: InstructionObserver(name) {}

/*******************************************************************************************/

class PrintObserver : public InstructionObserver {
public:
	DEFAULT_CONSTRUCTOR(PrintObserver);
	/*
	void load(IID iid, PTR addr, KVALUE* kv) {
		printf("<<<<< LOAD >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
												PTR_ToString(addr).c_str(),
												KVALUE_ToString(*kv).c_str());
	}
	*/


	// ***** Binary Operations ***** //

	virtual void add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< ADD >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< FADD >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< SUB >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< FSUB >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< MUL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< FMUL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< UDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< SDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< FDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< UREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< SREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< FREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}


	// ***** Bitwise Binary Operations ***** //
	virtual void shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	virtual void xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
															   (nuw ? "1" : "0"),
															   (nsw ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str(),
															   KVALUE_ToString(*op2).c_str());
	}

	// ***** Vector Operations ***** //

	virtual void extractelement() {
		printf("<<<<< EXTRACTELEMENT >>>>>\n");
	}

	virtual void insertelement() {
		printf("<<<<< INSERTELEMENT >>>>>\n");
	}

	virtual void shufflevector() {
		printf("<<<<< SHUFFLEVECTOR >>>>>\n");
	}


	// ***** AGGREGATE OPERATIONS ***** //

	virtual void extractvalue() {
		printf("<<<<< EXTRACTVALUE >>>>>\n");
	}

	virtual void insertvalue() {
		printf("<<<<< INSERTVALUE >>>>>\n");
	}


	// ***** Memory Access and Addressing Operations ***** //

	virtual void alloca() {
		printf("<<<<< ALLOCA >>>>>\n");
	}

	virtual void load() {
		printf("<<<<< LOAD >>>>>\n");
	}

	void store(IID iid, PTR addr, KVALUE* kv) {
		printf("<<<<< STORE >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
												 PTR_ToString(addr).c_str(),
												 KVALUE_ToString(*kv).c_str());
	}

	virtual void fence() {
		printf("<<<<< FENCE >>>>>\n");
	}

	virtual void cmpxchg() {
		printf("<<<<< CMPXCHG >>>>>\n");
	}

	virtual void atomicrmw() {
		printf("<<<<< ATOMICRMW >>>>>\n");
	}

	virtual void getelementptr() {
		printf("<<<<< GETELEMENTPTR >>>>>\n");
	}

	// ***** Conversion Operations ***** //

	virtual void trunc() {
		printf("<<<<< TRUNC >>>>>\n");
	}

	virtual void zext() {
		printf("<<<<< ZEXT >>>>>\n");
	}

	virtual void sext() {
		printf("<<<<< SEXT >>>>>\n");
	}

	virtual void fptrunc() {
		printf("<<<<< FPTRUNC >>>>>\n");
	}

	virtual void fpext() {
		printf("<<<<< FPEXT >>>>>\n");
	}

	virtual void fptoui() {
		printf("<<<<< FPTOUI >>>>>\n");
	}

	virtual void fptosi() {
		printf("<<<<< FPTOSI >>>>>\n");
	}

	virtual void uitofp() {
		printf("<<<<< UITOFP >>>>>\n");
	}

	virtual void sitofp() {
		printf("<<<<< SITOFP >>>>>\n");
	}

	virtual void ptrtoint() {
		printf("<<<<< PTRTOINT >>>>>\n");
	}

	virtual void inttoptr() {
		printf("<<<<< INTTOPTR >>>>>\n");
	}

	virtual void bitcast() {
		printf("<<<<< BITCAST >>>>>\n");
	}


	// ***** TerminatorInst ***** //
	virtual void branch(IID iid, bool conditional, KVALUE* op1) {
		printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s\n", IID_ToString(iid).c_str(),
															   (conditional ? "1" : "0"),
															   KVALUE_ToString(*op1).c_str());
	}

	virtual void branch2(IID iid, bool conditional) {
		printf("<<<<< BRANCH >>>>> %s, cond: %s\n", IID_ToString(iid).c_str(),
															   (conditional ? "1" : "0"));
	}

	virtual void indirectbr(IID iid, KVALUE* op1) {
		printf("<<<<< INDIRECTBR >>>>> %s, address: %s\n", IID_ToString(iid).c_str(),
        KVALUE_ToString(*op1).c_str());
	}

	virtual void invoke() {
		printf("<<<<< INVOKE >>>>>\n");
	}

	virtual void resume() {
		printf("<<<<< RESUME >>>>>\n");
	}

	virtual void return_() {
		printf("<<<<< RETURN >>>>>\n");
	}

	virtual void switch_() {
		printf("<<<<< SWITCH >>>>>\n");
	}

	virtual void unreachable() {
		printf("<<<<< UNREACHABLE >>>>>\n");
	}

	// ***** Other Operations ***** //

	virtual void icmp() {
		printf("<<<<< ICMP >>>>>\n");
	}

	virtual void fcmp() {
		printf("<<<<< FCMP >>>>>\n");
	}
	
	virtual void phinode() {
		printf("<<<<< PHINODE >>>>>\n");
	}

	virtual void select() {
		printf("<<<<< SELECT >>>>>\n");
	}

	virtual void call() {
		printf("<<<<< CALL >>>>>\n");
	}

	virtual void vaarg() {
		printf("<<<<< VAARG >>>>>\n");
	}

	virtual void landingpad() {
		printf("<<<<< LANDINGPAD >>>>>\n");
	}

};


#endif /* OBSERVERS_H_ */
