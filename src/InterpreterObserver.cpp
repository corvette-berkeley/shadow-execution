
#include "InterpreterObserver.h"
#include <stack>

using namespace std;
	
void InterpreterObserver::load(IID iid, PTR addr, IID addr_iid, KVALUE* kv) {
  printf("<<<<< LOAD >>>>> %s, %s %s, %s\n", IID_ToString(iid).c_str(),
	 PTR_ToString(addr).c_str(),
	 IID_ToString(addr_iid).c_str(),
	 KVALUE_ToString(*kv).c_str());
}

// ***** Binary Operations ***** //

void InterpreterObserver::add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< ADD >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FADD >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SUB >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FSUB >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< MUL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FMUL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< UDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< UREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}


// ***** Bitwise Binary Operations ***** //
void InterpreterObserver::shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

// ***** Vector Operations ***** //

void InterpreterObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< EXTRACTELEMENT >>>>> %s, vector:%s, index:%s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

void InterpreterObserver::insertelement() {
  printf("<<<<< INSERTELEMENT >>>>>\n");
}

void InterpreterObserver::shufflevector() {
  printf("<<<<< SHUFFLEVECTOR >>>>>\n");
}


// ***** AGGREGATE OPERATIONS ***** //

void InterpreterObserver::extractvalue(IID iid, KVALUE* op) {
  printf("<<<<< EXTRACTVALUE >>>>> %s, agg_val:%s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< INSERTVALUE >>>>> %s, vector:%s, value:%s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());
}

// ***** Memory Access and Addressing Operations ***** //

void InterpreterObserver::allocax(IID iid, int kind) {
  printf("<<<<< ALLOCA >>>>> %s, kind:%s\n", IID_ToString(iid).c_str(), KIND_ToString(kind).c_str());

  // create location
  Location *location = new Location(kind, true);

  // putting location in currentFrame
  (*currentFrame)[iid] = location;

  // printing current frame
  //printCurrentFrame();
}

void InterpreterObserver::store(IID iid, PTR addr, IID addr_iid, KVALUE* kv) {
  printf("<<<<< STORE >>>>> %s, %s %s, %s\n", IID_ToString(iid).c_str(),
	 PTR_ToString(addr).c_str(),
	 IID_ToString(addr_iid).c_str(),
	 KVALUE_ToString(*kv).c_str());
}

void InterpreterObserver::fence() {
  printf("<<<<< FENCE >>>>>\n");
}

void InterpreterObserver::cmpxchg() {
  printf("<<<<< CMPXCHG >>>>>\n");
}

void InterpreterObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2) {
  printf("<<<<< CMPXCHG >>>>> %s, %s, %s, %s\n", IID_ToString(iid).c_str(),
	 PTR_ToString(addr).c_str(),
	 KVALUE_ToString(*kv1).c_str(),
	 KVALUE_ToString(*kv2).c_str());
}

void InterpreterObserver::atomicrmw() {
  printf("<<<<< ATOMICRMW >>>>>\n");
}

void InterpreterObserver::getelementptr(IID iid, bool inbound, KVALUE* op) {
  printf("<<<<< GETELEMENTPTR >>>>> %s, inbound:%s, pointer_value:%s\n", IID_ToString(iid).c_str(),
	 (inbound ? "1" : "0"),
	 KVALUE_ToString(*op).c_str());
}

// ***** Conversion Operations ***** //

void InterpreterObserver::trunc(IID iid, KVALUE* op) {
  printf("<<<<< TRUNC >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::zext(IID iid, KVALUE* op) {
  printf("<<<<< ZEXT >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::sext(IID iid, KVALUE* op) {
  printf("<<<<< SEXT >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::fptrunc(IID iid, KVALUE* op) {
  printf("<<<<< FPTRUNC >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::fpext(IID iid, KVALUE* op) {
  printf("<<<<< FPEXT >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::fptoui(IID iid, KVALUE* op) {
  printf("<<<<< FPTOUII >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::fptosi(IID iid, KVALUE* op) {
  printf("<<<<< FPTOSI >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::uitofp(IID iid, KVALUE* op) {
  printf("<<<<< UITOFP >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::sitofp(IID iid, KVALUE* op) {
  printf("<<<<< SITOFP >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::ptrtoint(IID iid, KVALUE* op) {
  printf("<<<<< PTRTOINT >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::inttoptr(IID iid, KVALUE* op) {
  printf("<<<<< INTTOPTR >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::bitcast(IID iid, KVALUE* op) {
  printf("<<<<< BITCAST >>>>> %s, %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}


// ***** TerminatorInst ***** //
void InterpreterObserver::branch(IID iid, bool conditional, KVALUE* op1) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s\n", IID_ToString(iid).c_str(),
	 (conditional ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str());
}

void InterpreterObserver::branch2(IID iid, bool conditional) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s\n", IID_ToString(iid).c_str(),
	 (conditional ? "1" : "0"));
}

void InterpreterObserver::indirectbr(IID iid, KVALUE* op1) {
  printf("<<<<< INDIRECTBR >>>>> %s, address: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str());
}

void InterpreterObserver::invoke(IID iid, KVALUE* call_value) {
  printf("<<<<< INVOKE >>>>> %s, call_value: %s", IID_ToString(iid).c_str(), KVALUE_ToString(*call_value).c_str());
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    printf(", arg: %s", KVALUE_ToString(*value).c_str()); 
  }
  printf("\n");
  
}

void InterpreterObserver::resume(IID iid, KVALUE* op1) {
  printf("<<<<< RESUME >>>>> %s, acc_value: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str());
}

void InterpreterObserver::return_(IID iid, KVALUE* op1) {
  printf("<<<<< RETURN >>>>> %s, ret_value: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str());
}

void InterpreterObserver::return2_(IID iid) {
  printf("<<<<< RETURN >>>>> %s\n", IID_ToString(iid).c_str());
}

void InterpreterObserver::switch_(IID iid, KVALUE* op) {
  printf("<<<<< SWITCH >>>>> %s, condition: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());
}

void InterpreterObserver::unreachable() {
  printf("<<<<< UNREACHABLE >>>>>\n");
}

// ***** Other Operations ***** //

void InterpreterObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred) {
  printf("<<<<< ICMP >>>>> %s, %s, %s, %d\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred);
}

void InterpreterObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred) {
  printf("<<<<< FCMP >>>>> %s, %s, %s, %d\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred);
}

void InterpreterObserver::phinode() {
  printf("<<<<< PHINODE >>>>>\n");
}

void InterpreterObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue) {
  printf("<<<<< SELECT >>>>> %s, %s, %s, %s\n", IID_ToString(iid).c_str(), KVALUE_ToString(*cond).c_str(), KVALUE_ToString(*tvalue).c_str(), 
	 KVALUE_ToString(*fvalue).c_str());
}

void InterpreterObserver::select() {
  printf("<<<<< SELECT >>>>>\n");
}

void InterpreterObserver::push_stack(KVALUE* value) {
  myStack.push(value);
}

void InterpreterObserver::call(IID iid, KVALUE* call_value) {
  printf("<<<<< CALL >>>>> %s, call_value: %s", IID_ToString(iid).c_str(), KVALUE_ToString(*call_value).c_str());
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    printf(", arg: %s", KVALUE_ToString(*value).c_str()); 
  }
  printf("\n");
}

void InterpreterObserver::vaarg() {
  printf("<<<<< VAARG >>>>>\n");
}

void InterpreterObserver::landingpad() {
  printf("<<<<< LANDINGPAD >>>>>\n");
}

void InterpreterObserver::printCurrentFrame() {
  printf("Print current frame\n");
  for(Frame::iterator it = currentFrame->begin(); it != currentFrame->end(); it++) {
    printf("%ld \n", it->first);
  }
}
