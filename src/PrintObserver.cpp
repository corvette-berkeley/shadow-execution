
#include "PrintObserver.h"
#include <stack>

using namespace std;
	
void PrintObserver::load(IID iid, KVALUE* op, int inx) {
  printf("<<<<< LOAD >>>>> %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str(),
   inx);
}

// ***** Binary Operations ***** //

void PrintObserver::add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< ADD >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), 
   inx);
}

void PrintObserver::fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< FADD >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< SUB >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< FSUB >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< MUL >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< FMUL >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< UDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< SDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< FDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< UREM >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< SREM >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< FREM >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}


// ***** Bitwise Binary Operations ***** //
void PrintObserver::shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

// ***** Vector Operations ***** //

void PrintObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< EXTRACTELEMENT >>>>> %s, vector:%s, index:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

void PrintObserver::insertelement() {
  printf("<<<<< INSERTELEMENT >>>>>\n");
}

void PrintObserver::shufflevector() {
  printf("<<<<< SHUFFLEVECTOR >>>>>\n");
}


// ***** AGGREGATE OPERATIONS ***** //

void PrintObserver::extractvalue(IID iid, KVALUE* op, int inx) {
  printf("<<<<< EXTRACTVALUE >>>>> %s, agg_val:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< INSERTVALUE >>>>> %s, vector:%s, value:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str(), inx);
}

// ***** Memory Access and Addressing Operations ***** //

void PrintObserver::allocax(IID iid, KIND kind, uint64_t size, int inx) {
  printf("<<<<< ALLOCA >>>>> %s, kind:%s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(kind).c_str(), size, inx);
}

void PrintObserver::allocax_array(IID iid, KIND kind, uint64_t size, int inx) {
  printf("<<<<< ALLOCA >>>>> %s, kind:%s, size:%ld, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(kind).c_str(), size, inx);
}

void PrintObserver::allocax_struct(IID iid, uint64_t size, int inx) {
  printf("<<<<< ALLOCA STRUCT >>>>> %s, size: %ld, inx: %d\n", IID_ToString(iid).c_str(), size, inx);
}

void PrintObserver::store(IID iid, KVALUE* op, KVALUE* kv, int inx) {
  printf("<<<<< STORE >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str(),
	 KVALUE_ToString(*kv).c_str(), inx);
}

void PrintObserver::fence() {
  printf("<<<<< FENCE >>>>>\n");
}

void PrintObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx) {
  printf("<<<<< CMPXCHG >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 PTR_ToString(addr).c_str(),
	 KVALUE_ToString(*kv1).c_str(),
	 KVALUE_ToString(*kv2).c_str(), inx);
}

void PrintObserver::atomicrmw() {
  printf("<<<<< ATOMICRMW >>>>>\n");
}

void PrintObserver::getelementptr(IID iid, bool inbound, KVALUE* op, KVALUE* index, KIND kind, uint64_t size, int inx) {
  printf("<<<<< GETELEMENTPTR >>>>> %s, inbound:%s, pointer_value:%s, index:%s, kind: %s, size %ld, [INX: %d]\n", 
      IID_ToString(iid).c_str(),
	 (inbound ? "1" : "0"),
	 KVALUE_ToString(*op).c_str(),
	 KVALUE_ToString(*index).c_str(), 
	 KIND_ToString(kind).c_str(),
	 size,
	 inx);
}

void PrintObserver::getelementptr_array(IID iid, bool inbound, KVALUE* op, KIND kind, int inx) {
  printf("<<<<< GETELEMENTPTR ARRAY >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, [INX: %d]\n", 
      IID_ToString(iid).c_str(),
	 (inbound ? "1" : "0"),
	 KVALUE_ToString(*op).c_str(),
	 KIND_ToString(kind).c_str(),
	 inx);
}

void PrintObserver::getelementptr_struct(IID iid, bool inbound, KVALUE* op, KIND kind, int inx) {
  printf("<<<<< GETELEMENTPTR STRUCT >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, [INX: %d]\n", 
      IID_ToString(iid).c_str(),
	 (inbound ? "1" : "0"),
	 KVALUE_ToString(*op).c_str(),
	 KIND_ToString(kind).c_str(),
	 inx);
}

// ***** Conversion Operations ***** //

void PrintObserver::trunc(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< TRUNC >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::zext(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< ZEXT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::sext(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< SEXT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::fptrunc(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< FPTRUNC >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::fpext(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< FPEXT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::fptoui(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< FPTOUII >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::fptosi(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< FPTOSI >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::uitofp(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< UITOFP >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::sitofp(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< SITOFP >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::ptrtoint(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< PTRTOINT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::inttoptr(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< INTTOPTR >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::bitcast(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< BITCAST >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);
}


// ***** TerminatorInst ***** //
void PrintObserver::branch(IID iid, bool conditional, KVALUE* op1, int inx) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (conditional ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(), inx);
}

void PrintObserver::branch2(IID iid, bool conditional, int inx) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 (conditional ? "1" : "0"), inx);
}

void PrintObserver::indirectbr(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< INDIRECTBR >>>>> %s, address: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(), inx);
}

void PrintObserver::invoke(IID iid, KVALUE* call_value, int inx) {
  printf("<<<<< INVOKE >>>>> %s, call_value: %s", IID_ToString(iid).c_str(), KVALUE_ToString(*call_value).c_str());
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    printf(", arg: %s", KVALUE_ToString(*value).c_str()); 
  }
  printf("[INX: %d]\n", inx);
  
}

void PrintObserver::resume(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< RESUME >>>>> %s, acc_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(), inx);
}

void PrintObserver::return_(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< RETURN >>>>> %s, ret_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(), inx);
}

void PrintObserver::return2_(IID iid, int inx) {
  printf("<<<<< RETURN >>>>> %s, [INX: %d]\n", IID_ToString(iid).c_str(), inx);
}

void PrintObserver::switch_(IID iid, KVALUE* op, int inx) {
  printf("<<<<< SWITCH >>>>> %s, condition: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str(), inx);
}

void PrintObserver::unreachable() {
  printf("<<<<< UNREACHABLE >>>>>\n");
}

// ***** Other Operations ***** //

void PrintObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  printf("<<<<< ICMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred, inx);
}

void PrintObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  printf("<<<<< FCMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred, inx);
}

void PrintObserver::phinode() {
  printf("<<<<< PHINODE >>>>>\n");
}

void PrintObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {
  printf("<<<<< SELECT >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*cond).c_str(), KVALUE_ToString(*tvalue).c_str(), 
	 KVALUE_ToString(*fvalue).c_str(), inx);
}

void PrintObserver::push_stack(KVALUE* value) {
  printf("<<<<< PUSH VALUE >>>>> kvalue: %s\n", KVALUE_ToString(*value).c_str());
}

void PrintObserver::push_struct_type(KIND kind) {
  printf("<<<<< PUSH STRUCT TYPE >>>>> kind: %s\n", KIND_ToString(kind).c_str());
}

void PrintObserver::push_getelementptr_inx(KVALUE* value) {
  printf("<<<<< PUSH GETELEMENTPTR INX >>>>> kvalue: %s\n", KVALUE_ToString(*value).c_str());
}

void PrintObserver::push_array_size(uint64_t i) {
  printf("<<<<< PUSH ARRAY SIZE >>>>> size: %ld", i);
}

void PrintObserver::construct_array_type(uint64_t i) {
  printf("<<<<< CONSTRUCT ARRAY TYPE >>>>> size: %ld", i);
}

void PrintObserver::call_nounwind(KVALUE* value) {
  printf("<<<<< CALL NOUNWIND >>>>>");
  printf("<<<<< CALL NOUNWIND >>>>> kvalue: %s", KVALUE_ToString(*value).c_str()); 
}

void PrintObserver::create_stack_frame(int size) {
  printf("<<<<< CREATE STACK >>>>>");
  printf("<<<<< CREATE STACK >>>>> size: %d\n", size);
}

void PrintObserver::call(IID iid, bool nounwind, KIND type, KVALUE* call_value, int inx) {
  printf("<<<<< CALL >>>>> %s, call_value: %s, nounwind: %d, return type: %s", 
      IID_ToString(iid).c_str(), 
      KVALUE_ToString(*call_value).c_str(), 
      (nounwind ? 1 : 0),
      KIND_ToString(type).c_str()
      );
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    printf(", arg: %s", KVALUE_ToString(*value).c_str()); 
  }
  printf("[INX: %d]\n", inx);
}

void PrintObserver::vaarg() {
  printf("<<<<< VAARG >>>>>\n");
}

void PrintObserver::landingpad() {
  printf("<<<<< LANDINGPAD >>>>>\n");
}

