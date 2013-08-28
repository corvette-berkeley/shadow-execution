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
	void store(IID iid, PTR addr, KVALUE* kv) {
		printf("<<<<< STORE >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
												 PTR_ToString(addr).c_str(),
												 KVALUE_ToString(*kv).c_str());
	}

	virtual void add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
		printf("<<<<< ADD >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
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

	virtual void alloca() {
		printf("<<<<< ALLOCA >>>>>\n");
	}


	virtual void call() {
		printf("<<<<< CALL >>>>>\n");
	}

	// ****** CastInst ****** //

	virtual void bitcast() {
		printf("<<<<< BITCAST >>>>>\n");
	}

	virtual void fpext() {
		printf("<<<<< FPEXT >>>>>\n");
	}

	virtual void fptosi() {
		printf("<<<<< FPTOSI >>>>>\n");
	}

	virtual void fptoui() {
		printf("<<<<< FPTOUI >>>>>\n");
	}

	virtual void fptrunc() {
		printf("<<<<< FPTRUNC >>>>>\n");
	}


	virtual void load() {
		printf("<<<<< LOAD >>>>>\n");
	}

	// ***** TerminatorInst ***** //

	virtual void branch() {
		printf("<<<<< BRANCH >>>>>\n");
	}

	virtual void indirectbr() {
		printf("<<<<< INDIRECTBR >>>>>\n");
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

	// ***** CmpInst ***** //

	virtual void fcmp() {
		printf("<<<<< FCMP >>>>>\n");
	}
	
	virtual void icmp() {
		printf("<<<<< ICMP >>>>>\n");
	}
};


#endif /* OBSERVERS_H_ */
