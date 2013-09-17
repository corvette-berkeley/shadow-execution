
#include "InterpreterObserver.h"

#include <assert.h>
#include <stack>
#include <llvm/InstrTypes.h>
	
void InterpreterObserver::load(IID iid, PTR addr, IID addr_iid, KVALUE* kv) {
  printf("<<<<< LOAD >>>>> %s, %s %s, %s\n", IID_ToString(iid).c_str(),
	 PTR_ToString(addr).c_str(),
	 IID_ToString(addr_iid).c_str(),
	 KVALUE_ToString(*kv).c_str());

  Location *loc = (*currentFrame)[addr_iid];
  Location *nloc = new Location(loc->getType(), loc->getValue(), false);

  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";
  
  return;
}

// ***** Binary Operations ***** //

void InterpreterObserver::add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< ADD >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  Location *loc1 = (*currentFrame)[op1->iid];
  Location *loc2 = (*currentFrame)[op2->iid];
  int result = loc1->getValue().as_int + loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FADD >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  Location *loc1 = (*currentFrame)[op1->iid];
  Location *loc2 = (*currentFrame)[op2->iid];

  VALUE vresult;

  if (loc1->getType() == FLP32_KIND)
  {
    vresult.as_flp = (float) loc1->getValue().as_flp + (float) loc2->getValue().as_flp;
  }
  else if (loc1->getType() == FLP64_KIND)
  {
    vresult.as_flp = (double) loc1->getValue().as_flp + (double) loc2->getValue().as_flp;
  }
  else if (loc1->getType() == FLP80X86_KIND)
  {
    vresult.as_flp = loc1->getValue().as_flp + loc2->getValue().as_flp;
  }
  else 
  {
    cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << loc1->getType() << "\n";
    abort();
  }

  Location *nloc = new Location(loc1->getType(), vresult, false);
  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SUB >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::sub] => Unimplemented\n";
  abort();
}

void InterpreterObserver::fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FSUB >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::fsub] => Unimplemented\n";
  abort();
}

void InterpreterObserver::mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< MUL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::mul] => Unimplemented\n";
  abort();
}

void InterpreterObserver::fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FMUL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::fmul] => Unimplemented\n";
  abort();
}

void InterpreterObserver::udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< UDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::udiv] => Unimplemented\n";
  abort();
}

void InterpreterObserver::sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::sdiv] => Unimplemented\n";
  abort();
}

void InterpreterObserver::fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FDIV >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::fdiv] => Unimplemented\n";
  abort();
}

void InterpreterObserver::urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< UREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::urem] => Unimplemented\n";
  abort();
}

void InterpreterObserver::srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::srem] => Unimplemented\n";
  abort();
}

void InterpreterObserver::frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< FREM >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::frem] => Unimplemented\n";
  abort();
}


// ***** Bitwise Binary Operations ***** //
void InterpreterObserver::shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  Location *loc1 = (*currentFrame)[op1->iid];
  Location *loc2 = (*currentFrame)[op2->iid];
  int result = loc1->getValue().as_int << loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::lshr] => Unimplemented\n";
  abort();
}

void InterpreterObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::ashr] => Unimplemented\n";
  abort();
}

void InterpreterObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  Location *loc1 = (*currentFrame)[op1->iid];
  Location *loc2 = (*currentFrame)[op2->iid];
  int result = loc1->getValue().as_int & loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  Location *loc1 = (*currentFrame)[op1->iid];
  Location *loc2 = (*currentFrame)[op2->iid];
  int result = loc1->getValue().as_int | loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s\n", IID_ToString(iid).c_str(),
	 (nuw ? "1" : "0"),
	 (nsw ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  Location *loc1 = (*currentFrame)[op1->iid];
  Location *loc2 = (*currentFrame)[op2->iid];
  int result = loc1->getValue().as_int ^ loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

// ***** Vector Operations ***** //

void InterpreterObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< EXTRACTELEMENT >>>>> %s, vector:%s, index:%s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::extractelement] => Unimplemented\n";
  abort();
}

void InterpreterObserver::insertelement() {
  printf("<<<<< INSERTELEMENT >>>>>\n");

  cerr << "[InterpreterObserver::insertelement] => Unimplemented\n";
  abort();
}

void InterpreterObserver::shufflevector() {
  printf("<<<<< SHUFFLEVECTOR >>>>>\n");

  cerr << "[InterpreterObserver::shufflevector] => Unimplemented\n";
  abort();
}


// ***** AGGREGATE OPERATIONS ***** //

void InterpreterObserver::extractvalue(IID iid, KVALUE* op) {
  printf("<<<<< EXTRACTVALUE >>>>> %s, agg_val:%s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::extractvalue] => Unimplemented\n";
  abort();
}

void InterpreterObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2) {
  printf("<<<<< INSERTVALUE >>>>> %s, vector:%s, value:%s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str(),
	 KVALUE_ToString(*op2).c_str());

  cerr << "[InterpreterObserver::insertvalue] => Unimplemented\n";
  abort();
}

// ***** Memory Access and Addressing Operations ***** //

void InterpreterObserver::allocax(IID iid, KIND type) {
  printf("<<<<< ALLOCA >>>>> %s, kind:%s\n", IID_ToString(iid).c_str(), KIND_ToString(type).c_str());

  Location *location = new Location(type, true);
  (*currentFrame)[iid] = location;
  cout << location->toString() << "\n";

  return;
}

bool checkStore(Location *dest, KVALUE *kv) {
  bool result = false;

  switch(kv->kind) {
  case PTR_KIND:
    result = (dest->getValue().as_ptr == kv->value.as_ptr);
    break;
  case INT1_KIND: 
  case INT8_KIND: 
  case INT16_KIND: 
  case INT32_KIND: 
  case INT64_KIND:
    result = (dest->getValue().as_int == kv->value.as_int);
    break;
  case FLP32_KIND:
    result = ((float)dest->getValue().as_flp) == ((float)kv->value.as_flp);
    break;
  case FLP64_KIND:
    result = ((double)dest->getValue().as_flp) == ((double)kv->value.as_flp);
    break;
  case FLP80X86_KIND:
    result = dest->getValue().as_flp == kv->value.as_flp;
    break;
  default: //safe_assert(false);
    break;
  }

  return result;
}

void InterpreterObserver::store(IID iid, PTR addr, IID addr_iid, KVALUE* kv) {
  printf("<<<<< STORE >>>>> %s, %s %s, %s\n", IID_ToString(iid).c_str(),
	 PTR_ToString(addr).c_str(),
	 IID_ToString(addr_iid).c_str(),
	 KVALUE_ToString(*kv).c_str());

  // retrieve Location to store in
  Location *dest = (*currentFrame)[addr_iid];
  cout << "Dest: " << dest->toString() << "\n";

  // the value to store is a constant
  if (kv->iid == 0) {
    dest->setValue(kv->value);
  }
  else {
    if (currentFrame->find(kv->iid) != currentFrame->end()) {
      Location *src = (*currentFrame)[kv->iid];
      dest->setValue(src->getValue());
    }
    else {
      cerr << "[InterpreterObserver::store] => Could not find map location\n";
      abort();
    }
  }
  cout << "Updated Dest: " << dest->toString() << "\n";
  

  if (!checkStore(dest, kv)) {
    cerr << "Mismatched values found in Store\n";
    abort();
  }

  return;
}

void InterpreterObserver::fence() {
  printf("<<<<< FENCE >>>>>\n");

  cerr << "[InterpreterObserver::fence] => Unimplemented\n";
  abort();
}

void InterpreterObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2) {
  printf("<<<<< CMPXCHG >>>>> %s, %s, %s, %s\n", IID_ToString(iid).c_str(),
	 PTR_ToString(addr).c_str(),
	 KVALUE_ToString(*kv1).c_str(),
	 KVALUE_ToString(*kv2).c_str());

  cerr << "[InterpreterObserver::cmpxchg] => Unimplemented\n";
  abort();
}

void InterpreterObserver::atomicrmw() {
  printf("<<<<< ATOMICRMW >>>>>\n");

  cerr << "[InterpreterObserver::atomicrmw] => Unimplemented\n";
  abort();
}

void InterpreterObserver::getelementptr(IID iid, bool inbound, KVALUE* op) {
  printf("<<<<< GETELEMENTPTR >>>>> %s, inbound:%s, pointer_value:%s\n", IID_ToString(iid).c_str(),
	 (inbound ? "1" : "0"),
	 KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::getelementptr] => Unimplemented\n";
  abort();
}

// ***** Conversion Operations ***** //

void InterpreterObserver::trunc(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< TRUNC >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::trunc] => Unimplemented\n";
  abort();
}

void InterpreterObserver::zext(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< ZEXT >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::zext] => Unimplemented\n";
  abort();
}

void InterpreterObserver::sext(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< SEXT >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::sext] => Unimplemented\n";
  abort();
}

void InterpreterObserver::fptrunc(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< FPTRUNC >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::fptrunc] => Unimplemented\n";
  abort();
}

void InterpreterObserver::fpext(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< FPEXT >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::fpext] => Unimplemented\n";
  abort();
}

void InterpreterObserver::fptoui(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< FPTOUII >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::fptoui] => Unimplemented\n";
  abort();
}

void InterpreterObserver::fptosi(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< FPTOSI >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::fptosi] => Unimplemented\n";
  abort();
}

void InterpreterObserver::uitofp(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< UITOFP >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::uitofp] => Unimplemented\n";
  abort();
}

void InterpreterObserver::sitofp(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< SITOFP >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::sitofp] => Unimplemented\n";
  abort();
}

void InterpreterObserver::ptrtoint(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< PTRTOINT >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::ptrtoint] => Unimplemented\n";
  abort();
}

void InterpreterObserver::inttoptr(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< INTTOPTR >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::inttoptr] => Unimplemented\n";
  abort();
}

void InterpreterObserver::bitcast(IID iid, KIND type, KVALUE* op) {
  printf("<<<<< BITCAST >>>>> %s, %s, %s\n", IID_ToString(iid).c_str(),
	 KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::bitcast] => Unimplemented\n";
  abort();
}


// ***** TerminatorInst ***** //
void InterpreterObserver::branch(IID iid, bool conditional, KVALUE* op1) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s\n", IID_ToString(iid).c_str(),
	 (conditional ? "1" : "0"),
	 KVALUE_ToString(*op1).c_str());

  cerr << "[InterpreterObserver::branch] => Unimplemented\n";
  abort();
}

void InterpreterObserver::branch2(IID iid, bool conditional) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s\n", IID_ToString(iid).c_str(),
	 (conditional ? "1" : "0"));

  cerr << "[InterpreterObserver::branch2] => Unimplemented\n";
  abort();
}

void InterpreterObserver::indirectbr(IID iid, KVALUE* op1) {
  printf("<<<<< INDIRECTBR >>>>> %s, address: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str());

  cerr << "[InterpreterObserver::indirectbr] => Unimplemented\n";
  abort();
}

void InterpreterObserver::invoke(IID iid, KVALUE* call_value) {
  printf("<<<<< INVOKE >>>>> %s, call_value: %s", IID_ToString(iid).c_str(), KVALUE_ToString(*call_value).c_str());
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    printf(", arg: %s", KVALUE_ToString(*value).c_str()); 
  }
  printf("\n");

  cerr << "[InterpreterObserver::invoke] => Unimplemented\n";
  abort();
}

void InterpreterObserver::resume(IID iid, KVALUE* op1) {
  printf("<<<<< RESUME >>>>> %s, acc_value: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str());

  cerr << "[InterpreterObserver::resume] => Unimplemented\n";
  abort();
}

void InterpreterObserver::return_(IID iid, KVALUE* op1) {
  printf("<<<<< RETURN >>>>> %s, ret_value: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op1).c_str());

  executionStack.pop();

  if (!executionStack.empty()) {
    currentFrame = executionStack.top();
    cout << "New stack size: " << executionStack.size() << "\n";
  }
  else {
    currentFrame = NULL;
    cout << "The execution stack is empty.\n";
  }
  
  return;
}

void InterpreterObserver::return2_(IID iid) {
  printf("<<<<< RETURN >>>>> %s\n", IID_ToString(iid).c_str());

  cerr << "[InterpreterObserver::return2] => Unimplemented\n";
  abort();
}

void InterpreterObserver::switch_(IID iid, KVALUE* op) {
  printf("<<<<< SWITCH >>>>> %s, condition: %s\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str());

  cerr << "[InterpreterObserver::switch] => Unimplemented\n";
  abort();
}

void InterpreterObserver::unreachable() {
  printf("<<<<< UNREACHABLE >>>>>\n");

  cerr << "[InterpreterObserver::unreachable] => Unimplemented\n";
  abort();
}

// ***** Other Operations ***** //

void InterpreterObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred) {
  printf("<<<<< ICMP >>>>> %s, %s, %s, %d\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred);

  Location *loc1 = (*currentFrame)[op1->iid];
  Location *loc2 = (*currentFrame)[op2->iid];

  int result = 0;
  switch(pred) {
  case CmpInst::ICMP_EQ:
    result = loc1->getValue().as_int == loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_NE:
    result = loc1->getValue().as_int != loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_UGT:
    result = loc1->getValue().as_int > loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_UGE:
    result = loc1->getValue().as_int >= loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_ULT:
    result = loc1->getValue().as_int < loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_ULE:
    result = loc1->getValue().as_int <= loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_SGT:
    result = loc1->getValue().as_int > loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_SGE:
    result = loc1->getValue().as_int >= loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_SLT:
    result = loc1->getValue().as_int < loc2->getValue().as_int;
    break;
  case CmpInst::ICMP_SLE:
    result = loc1->getValue().as_int <= loc2->getValue().as_int;
    break;
  default:
    break;
  }

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  (*currentFrame)[iid] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred) {
  printf("<<<<< FCMP >>>>> %s, %s, %s, %d\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred);

  cerr << "[InterpreterObserver::fcmp] => Unimplemented\n";
  abort();
}

void InterpreterObserver::phinode() {
  printf("<<<<< PHINODE >>>>>\n");
}

void InterpreterObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue) {
  printf("<<<<< SELECT >>>>> %s, %s, %s, %s\n", IID_ToString(iid).c_str(), KVALUE_ToString(*cond).c_str(), KVALUE_ToString(*tvalue).c_str(), 
	 KVALUE_ToString(*fvalue).c_str());

  cerr << "[InterpreterObserver::select] => Unimplemented\n";
  abort();
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

  cerr << "[InterpreterObserver::call] => Unimplemented\n";
  abort();
}

void InterpreterObserver::vaarg() {
  printf("<<<<< VAARG >>>>>\n");

  cerr << "[InterpreterObserver::vaarg] => Unimplemented\n";
  abort();
}

void InterpreterObserver::landingpad() {
  printf("<<<<< LANDINGPAD >>>>>\n");

  cerr << "[InterpreterObserver::landingpad] => Unimplemented\n";
  abort();
}

void InterpreterObserver::printCurrentFrame() {
  printf("Print current frame\n");
  for(Frame::iterator it = currentFrame->begin(); it != currentFrame->end(); it++) {
    printf("%ld \n", it->first);
  }
}
