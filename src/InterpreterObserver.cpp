
#include "InterpreterObserver.h"

#include <assert.h>
#include <stack>
#include <llvm/InstrTypes.h>
	
void InterpreterObserver::load(IID iid, KVALUE* op, KVALUE* kv, int inx) {
  printf("<<<<< LOAD >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
	 KVALUE_ToString(*op).c_str(),
	 KVALUE_ToString(*kv).c_str(),
   inx);

  Location *nloc;

  Location *loc = currentFrame[op->inx];
  if (loc->getType() == PTR_KIND) {
    nloc = static_cast<Location*>(loc->getValue().as_ptr);
  } else {
    nloc = new Location(loc->getType(), loc->getValue(), false);
  }

  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";
  
  return;
}

// ***** Binary Operations ***** //

std::string InterpreterObserver::BINOP_ToString(int binop) {
	std::stringstream s;
	switch(binop) {
    case ADD_OP:
			s << "ADD";
			break;
    case FADD_OP:
      s << "FADD";
      break;
    case SUB_OP:
      s << "SUB";
      break;
    case FSUB_OP:
      s << "FSUB";
      break;
    case MUL_OP:
      s << "MUL";
      break;
    case FMUL_OP:
      s << "FMUL";
      break;
    case UDIV_OP:
      s << "UDIV";
      break;
    case SDIV_OP:
      s << "SDIV";
      break;
    case FDIV_OP:
      s << "FDIV";
      break;
    case UREM_OP:
      s << "UREM";
      break;
    case SREM_OP:
      s << "SREM";
      break;
    case FREM_OP:
      s << "FREM";
      break;
		default: 
      safe_assert(false);
			break;
	}
	return s.str();
}

  long double InterpreterObserver::getValueFromConstant(KVALUE* op) {
    KIND kind = op->kind;
    if (kind == INT1_KIND || kind == INT8_KIND || kind == INT16_KIND || kind == INT32_KIND || kind == INT64_KIND) {
      return op->value.as_int;
    } else {
      return op->value.as_flp;
    }
  }

  long double InterpreterObserver::getValueFromLocation(Location* loc) {
    KIND kind = loc->getType();
    if (kind == INT1_KIND || kind == INT8_KIND || kind == INT16_KIND || kind == INT32_KIND || kind == INT64_KIND) {
      return loc->getValue().as_int;
    } else {
      return loc->getValue().as_flp;
    }
  }

void InterpreterObserver::binop(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx, BINOP op) {
  printf("<<<<< %s >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", 
      BINOP_ToString(op).c_str(),
      IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(),
      inx);

  long double v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    Location *loc1 = currentFrame[op1->inx];
    v1 = getValueFromLocation(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    Location *loc2 = currentFrame[op2->inx];
    v2 = getValueFromLocation(loc2);
  }

  VALUE vresult;
  switch (op) {
    case ADD_OP:
      vresult.as_int = (int) v1 + (int) v2;
      break;
    case SUB_OP:
      vresult.as_int = (int) v1 - (int) v2;
      break;
    case MUL_OP:
      vresult.as_int = (int) v1 * (int) v2;
      break;
    case UDIV_OP:
      vresult.as_int = (int) v1 / (int) v2;
      break;
    case SDIV_OP:
      vresult.as_int = (int) v1 / (int) v2;
      break;
    case UREM_OP:
      vresult.as_int = (int) v1 % (int) v2;
      break;
    case SREM_OP:
      vresult.as_int = (int) v1 % (int) v2;
      break;
    case FADD_OP:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 + (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 + (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 + v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FSUB_OP:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 - (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 - (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 - v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FMUL_OP:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 * (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 * (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 * v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    case FDIV_OP:
      switch(op1->kind) {
        case FLP32_KIND: 
          vresult.as_flp = (float) v1 / (float) v2;
          break;
        case FLP64_KIND:
          vresult.as_flp = (double) v1 / (double) v2;
          break;
        case FLP80X86_KIND:
          vresult.as_flp = v1 / v2;
          break;
        default:
          cerr << "[InterpreterObserver::fadd] => Unsupported floating-point type " << op1->kind << "\n";
          abort();
      }
      break;
    default:
      cerr << "[InterpreterObserver::binop] => Unsupported bin op " << BINOP_ToString(op) << "\n";
      abort();
  }

  Location *nloc = new Location(op1->kind, vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, ADD_OP);
}

void InterpreterObserver::fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FADD_OP);
}

void InterpreterObserver::sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, SUB_OP);
}

void InterpreterObserver::fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FSUB_OP);
}

void InterpreterObserver::mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, MUL_OP);
}

void InterpreterObserver::fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FMUL_OP);
}

void InterpreterObserver::udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, UDIV_OP);
}

void InterpreterObserver::sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, SDIV_OP);
}

void InterpreterObserver::fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, FDIV_OP);
}

void InterpreterObserver::urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, UREM_OP);
}

void InterpreterObserver::srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  binop(iid, nuw, nsw, op1, op2, inx, SREM_OP);
}

void InterpreterObserver::frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< FREM >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  cerr << "[InterpreterObserver::extractvalue] => Unsupported in C???\n";
  abort();
}


// ***** Bitwise Binary Operations ***** //
void InterpreterObserver::shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< SHL >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Location *loc1 = currentFrame[op1->inx];
  Location *loc2 = currentFrame[op2->inx];
  int result = loc1->getValue().as_int << loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< LSHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Location *loc1 = currentFrame[op1->inx];
  Location *loc2 = currentFrame[op2->inx];
  int result = loc1->getValue().as_int >> loc2->getValue().as_int; // same as arith?

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< ASHR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Location *loc1 = currentFrame[op1->inx];
  Location *loc2 = currentFrame[op2->inx];
  int result = loc1->getValue().as_int >> loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< AND >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Location *loc1 = currentFrame[op1->inx];
  Location *loc2 = currentFrame[op2->inx];
  int result = loc1->getValue().as_int & loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< OR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Location *loc1 = currentFrame[op1->inx];
  Location *loc2 = currentFrame[op2->inx];
  int result = loc1->getValue().as_int | loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< XOR >>>>> %s, nuw:%s, nsw:%s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (nuw ? "1" : "0"),
      (nsw ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  Location *loc1 = currentFrame[op1->inx];
  Location *loc2 = currentFrame[op2->inx];
  int result = loc1->getValue().as_int ^ loc2->getValue().as_int;

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(loc1->getType(), vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

// ***** Vector Operations ***** //

void InterpreterObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< EXTRACTELEMENT >>>>> %s, vector:%s, index:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

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

void InterpreterObserver::extractvalue(IID iid, KVALUE* op, int inx) {
  printf("<<<<< EXTRACTVALUE >>>>> %s, agg_val:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op).c_str(), 
      inx);

  cerr << "[InterpreterObserver::extractvalue] => Unimplemented\n";
  abort();
}

void InterpreterObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  printf("<<<<< INSERTVALUE >>>>> %s, vector:%s, value:%s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(),
      KVALUE_ToString(*op2).c_str(), inx);

  cerr << "[InterpreterObserver::insertvalue] => Unimplemented\n";
  abort();
}

// ***** Memory Access and Addressing Operations ***** //

void InterpreterObserver::allocax(IID iid, KIND type, uint64_t size, int inx) {
  printf("<<<<< ALLOCA >>>>> %s, kind:%s, [INX: %d]\n", IID_ToString(iid).c_str(), KIND_ToString(type).c_str(), inx);

  Location* location;
  if (callArgs.empty()) {
    if (type == ARRAY_KIND) {
      Location** locArr = (Location**) malloc(size*sizeof(Location*));
      void* locArrAdr = (void*) locArr;
      VALUE locArrVal;
      locArrVal.as_ptr = locArrAdr;
      location = new Location(ARRAY_KIND, locArrVal, true);
      currentFrame[inx] = location;
    } else {
      location = new Location(type, true);
      currentFrame[inx] = location;
    }
  } else {
    safe_assert(!callArgs.empty());
    location = callArgs.top();
    currentFrame[inx] = location;
    callArgs.pop();
  }

  cout << location->toString() << "\n";

  return;
}

bool checkStore(Location *dest, KVALUE *kv) {
  bool result = false;

  switch(kv->kind) {
    case PTR_KIND:
      result = true; // don't compare pointer
      // result = (dest->getValue().as_ptr == kv->value.as_ptr);
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

void InterpreterObserver::store(IID iid, KVALUE* op, KVALUE* kv, int inx) {
  printf("<<<<< STORE >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op).c_str(),
      KVALUE_ToString(*kv).c_str(), inx);

  // retrieve Location to store in
  Location *dest = currentFrame[op->inx];
  cout << "Dest: " << dest->toString() << "\n";

  // the value to store is a constant
  if (kv->iid == 0) {
    dest->setValue(kv->value);
  }
  else {
    Location* src = currentFrame[kv->inx];
    if (dest->getType() == PTR_KIND) {
      void* srcAdr = src;
      VALUE destVal = dest->getValue();
      destVal.as_ptr = srcAdr;
      dest->setValue(destVal);
    } else {
      dest->setValue(src->getValue());
    }
  }

  cout << "Updated Dest: " << dest->toString() << "\n";


  if (!checkStore(dest, kv)) {
    cerr << "KVALUE: " << KVALUE_ToString(*kv) << "\n";
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

void InterpreterObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx) {
  printf("<<<<< CMPXCHG >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      PTR_ToString(addr).c_str(),
      KVALUE_ToString(*kv1).c_str(),
      KVALUE_ToString(*kv2).c_str(), 
      inx);

  cerr << "[InterpreterObserver::cmpxchg] => Unimplemented\n";
  abort();
}

void InterpreterObserver::atomicrmw() {
  printf("<<<<< ATOMICRMW >>>>>\n");

  cerr << "[InterpreterObserver::atomicrmw] => Unimplemented\n";
  abort();
}

void InterpreterObserver::getelementptr(IID iid, bool inbound, KVALUE* op, KIND kind, uint64_t size, int inx) {
  printf("<<<<< GETELEMENTPTR >>>>> %s, inbound:%s, pointer_value:%s, kind: %s, size: %ld, [INX: %d]\n", 
      IID_ToString(iid).c_str(),
      (inbound ? "1" : "0"),
      KVALUE_ToString(*op).c_str(), 
      KIND_ToString(kind).c_str(),
      size,
      inx);

  cerr << "[InterpreterObserver::getelementptr] => Unimplemented\n";
  abort();
}

// ***** Conversion Operations ***** //

void InterpreterObserver::trunc(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< TRUNC >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);

  cerr << "[InterpreterObserver::trunc] => Unimplemented\n";
  abort();
}

void InterpreterObserver::zext(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< ZEXT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);

  cerr << "[InterpreterObserver::zext] => Unimplemented\n";
  abort();
}

void InterpreterObserver::sext(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< SEXT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);

  Location *src = currentFrame[op->inx];
  VALUE value = src->getValue();

  VALUE ext_value;

  switch (type) {
    case INT1_KIND:
      ext_value.as_int = (char) value.as_int;
      break;
    case INT8_KIND:
      ext_value.as_int = (int8_t) value.as_int;
      break;
    case INT16_KIND:
      ext_value.as_int = (int16_t) value.as_int;
      break;
    case INT32_KIND: 
      ext_value.as_int = (int32_t) value.as_int;
      break;
    case INT64_KIND:
      ext_value.as_int = (int64_t) value.as_int;
      break;
    default:
    cerr << "[InterpreterObserver::sext] => Unsupport integer type " << type << "\n";
    abort();
  }

  Location *ext_loc = new Location(type, ext_value, false);
  currentFrame[inx] = ext_loc;
  cout << ext_loc->toString() << "\n";
}

void InterpreterObserver::fptrunc(IID iid, KIND type, KVALUE* kv, int inx) {
  printf("<<<<< FPTRUNC >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), inx);

  Location *src = currentFrame[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_flp;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_flp;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_flp;
  }
  else 
  {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Location *trunc_loc = new Location(type, trunc_value, false);
  currentFrame[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fpext(IID iid, KIND type, KVALUE* kv, int inx) {
  printf("<<<<< FPEXT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), inx);

  Location *src = currentFrame[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_flp;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_flp;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_flp;
  }
  else 
  {
    cerr << "[InterpreterObserver::fpext] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Location *trunc_loc = new Location(type, trunc_value, false);
  currentFrame[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fptoui(IID iid, KIND type, KVALUE* kv, int inx) {
  printf("<<<<< FPTOUII >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), inx);

  Location *src = currentFrame[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  Location *trunc_loc = new Location(type, trunc_value, false);
  currentFrame[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::fptosi(IID iid, KIND type, KVALUE* kv, int inx) {
  printf("<<<<< FPTOSI >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), inx);

  Location *src = currentFrame[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;
  trunc_value.as_int = (int) value.as_flp;

  Location *trunc_loc = new Location(type, trunc_value, false);
  currentFrame[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::uitofp(IID iid, KIND type, KVALUE* kv, int inx) {
  printf("<<<<< UITOFP >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), inx);

  Location *src = currentFrame[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_int;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_int;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_int;
  }
  else 
  {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Location *trunc_loc = new Location(type, trunc_value, false);
  currentFrame[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";
}

void InterpreterObserver::sitofp(IID iid, KIND type, KVALUE* kv, int inx) {
  printf("<<<<< SITOFP >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*kv).c_str(), inx);

  Location *src = currentFrame[kv->inx];
  VALUE value = src->getValue();

  VALUE trunc_value;

  if (type == FLP32_KIND)
  {
    trunc_value.as_flp = (float) value.as_int;
  }
  else if (type == FLP64_KIND)
  {
    trunc_value.as_flp = (double) value.as_int;
  }
  else if (type == FLP80X86_KIND)
  {
    trunc_value.as_flp = (long double) value.as_int;
  }
  else 
  {
    cerr << "[InterpreterObserver::fptrunc] => Unsupport floating point type " << type << "\n";
    abort();
  }

  Location *trunc_loc = new Location(type, trunc_value, false);
  currentFrame[inx] = trunc_loc;
  cout << trunc_loc->toString() << "\n";

}

void InterpreterObserver::ptrtoint(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< PTRTOINT >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);

  cerr << "[InterpreterObserver::ptrtoint] => Unimplemented\n";
  abort();
}

void InterpreterObserver::inttoptr(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< INTTOPTR >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);

  cerr << "[InterpreterObserver::inttoptr] => Unimplemented\n";
  abort();
}

void InterpreterObserver::bitcast(IID iid, KIND type, KVALUE* op, int inx) {
  printf("<<<<< BITCAST >>>>> %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KIND_ToString(type).c_str(), KVALUE_ToString(*op).c_str(), inx);

  cerr << "[InterpreterObserver::bitcast] => Unimplemented\n";
  abort();
}


// ***** TerminatorInst ***** //
void InterpreterObserver::branch(IID iid, bool conditional, KVALUE* op1, int inx) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, cond_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (conditional ? "1" : "0"),
      KVALUE_ToString(*op1).c_str(), inx);
}

void InterpreterObserver::branch2(IID iid, bool conditional, int inx) {
  printf("<<<<< BRANCH >>>>> %s, cond: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      (conditional ? "1" : "0"), inx);
}

void InterpreterObserver::indirectbr(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< INDIRECTBR >>>>> %s, address: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(), inx);
}

void InterpreterObserver::invoke(IID iid, KVALUE* call_value, int inx) {
  printf("<<<<< INVOKE >>>>> %s, call_value: %s, [INX: %d]", IID_ToString(iid).c_str(), KVALUE_ToString(*call_value).c_str(), inx);
  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    myStack.pop();
    printf(", arg: %s", KVALUE_ToString(*value).c_str()); 
  }
  printf("\n");

  cerr << "[InterpreterObserver::invoke] => Unimplemented\n";
  abort();
}

void InterpreterObserver::resume(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< RESUME >>>>> %s, acc_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(), inx);

  cerr << "[InterpreterObserver::resume] => Unimplemented\n";
  abort();
}

void InterpreterObserver::return_(IID iid, KVALUE* op1, int inx) {
  printf("<<<<< RETURN >>>>> %s, ret_value: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op1).c_str(), inx);

  safe_assert(!executionStack.empty());
  executionStack.pop();

  if (!executionStack.empty()) {
    currentFrame = executionStack.top();
    cout << "New stack size: " << executionStack.size() << "\n";
    if (!callerVarIndex.empty()) {
      currentFrame[callerVarIndex.top()]->setValue(op1->value); 
      cout << currentFrame[callerVarIndex.top()]->toString() << "\n";

      safe_assert(!callerVarIndex.empty());
      callerVarIndex.pop();
    }
  } else {
    currentFrame = NULL;
    cout << "The execution stack is empty.\n";
  }

  return;
}

void InterpreterObserver::return2_(IID iid, int inx) {
  printf("<<<<< RETURN >>>>> %s, [INX: %d]\n", IID_ToString(iid).c_str(), inx);

  safe_assert(!executionStack.empty());
  executionStack.pop();

  if (!executionStack.empty()) {
    currentFrame = executionStack.top();
    cout << "New stack size: " << executionStack.size() << "\n";
  } else {
    currentFrame = NULL;
    cout << "The execution stack is empty.\n";
  }

  safe_assert(!callerVarIndex.empty());
  callerVarIndex.pop();

  return;
}

void InterpreterObserver::switch_(IID iid, KVALUE* op, int inx) {
  printf("<<<<< SWITCH >>>>> %s, condition: %s, [INX: %d]\n", IID_ToString(iid).c_str(),
      KVALUE_ToString(*op).c_str(), inx);
}

void InterpreterObserver::unreachable() {
  printf("<<<<< UNREACHABLE >>>>>\n");
}

// ***** Other Operations ***** //

void InterpreterObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  printf("<<<<< ICMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred, inx);

  int v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    Location *loc1 = currentFrame[op1->inx];
    v1 = getValueFromLocation(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    Location *loc2 = currentFrame[op2->inx];
    v2 = getValueFromLocation(loc2);
  }

  int result = 0;
  switch(pred) {
    case CmpInst::ICMP_EQ:
      result = v1 == v2;
      break;
    case CmpInst::ICMP_NE:
      result = v1 != v2;
      break;
    case CmpInst::ICMP_UGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_UGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_ULT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_ULE:
      result = v1 <= v2;
      break;
    case CmpInst::ICMP_SGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_SGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_SLT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_SLE:
      result = v1 <= v2;
      break;
    default:
      break;
  }

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(INT1_KIND, vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  printf("<<<<< FCMP >>>>> %s, %s, %s, %d, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*op1).c_str(), KVALUE_ToString(*op2).c_str(), pred, inx);


  long double v1, v2;

  // get value of v1
  if (op1->iid == 0) { // constant
    v1 = getValueFromConstant(op1);
  } else { // register
    Location *loc1 = currentFrame[op1->inx];
    v1 = getValueFromLocation(loc1);
  }

  // get value of v2
  if (op2->iid == 0) { // constant
    v2 = getValueFromConstant(op2);
  } else { // register
    Location *loc2 = currentFrame[op2->inx];
    v2 = getValueFromLocation(loc2);
  }

  int result = 0;
  switch(pred) {
    case CmpInst::ICMP_EQ:
      result = v1 == v2;
      break;
    case CmpInst::ICMP_NE:
      result = v1 != v2;
      break;
    case CmpInst::ICMP_UGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_UGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_ULT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_ULE:
      result = v1 <= v2;
      break;
    case CmpInst::ICMP_SGT:
      result = v1 > v2;
      break;
    case CmpInst::ICMP_SGE:
      result = v1 >= v2;
      break;
    case CmpInst::ICMP_SLT:
      result = v1 < v2;
      break;
    case CmpInst::ICMP_SLE:
      result = v1 <= v2;
      break;
    default:
      break;
  }

  // put result back to VALUE
  // TODO: incomplete?!
  VALUE vresult;
  vresult.as_int = result;

  Location *nloc = new Location(INT1_KIND, vresult, false);
  currentFrame[inx] = nloc;
  cout << nloc->toString() << "\n";

  return;
}

void InterpreterObserver::phinode() {
  printf("<<<<< PHINODE >>>>>\n");
}

void InterpreterObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {
  printf("<<<<< SELECT >>>>> %s, %s, %s, %s, [INX: %d]\n", IID_ToString(iid).c_str(), KVALUE_ToString(*cond).c_str(), KVALUE_ToString(*tvalue).c_str(), 
      KVALUE_ToString(*fvalue).c_str(), inx);

  cerr << "[InterpreterObserver::select] => Unimplemented\n";
  abort();
}

void InterpreterObserver::push_stack(KVALUE* value) {
  printf("<<<<< PUSH ARGS TO STACK >>>>>\n");
  myStack.push(value);
}

void InterpreterObserver::construct_array_type(uint64_t i) {
  printf("<<<<< CONSTRUCT ARRAY TYPE >>>>>: %ld\n", i);
  arrayType.push(i);
}

void InterpreterObserver::call_nounwind(KVALUE* kvalue) {
  printf("<<<<< CALL NOUNWIND >>>>>\n");
  safe_assert(!callerVarIndex.empty());
  Location* reg = currentFrame[callerVarIndex.top()];
  reg->setValue(kvalue->value);
  callerVarIndex.pop();
}

void InterpreterObserver::nreate_stack_frame(int size) {
  printf("<<<<< CREATE STACK FRAME >>>>>\n");
  currentFrame = (Location**) malloc(sizeof(Location*)*size);
  executionStack.push(currentFrame);
}

void InterpreterObserver::call(IID iid, bool nounwind, KIND type, KVALUE* call_value, int inx) {
  // debugging
  printf("<<<<< CALL >>>>> %s, call_value: %s, return type: %s, nounwind: %d, [INX: %d]", 
      IID_ToString(iid).c_str(), 
      KVALUE_ToString(*call_value).c_str(), 
      KIND_ToString(type).c_str(), 
      (nounwind ? 1 : 0),
      inx);

  if (nounwind) {
    while (!myStack.empty()) {
      myStack.pop();
    }
    while (!callArgs.empty()) {
      callArgs.pop();
    }
  } else {
    while (!myStack.empty()) {
      KVALUE* value = myStack.top();
      myStack.pop();

      // debugging
      printf(", arg: %s", KVALUE_ToString(*value).c_str()); 

      Location* arg = currentFrame[value->inx];
      Location* argCopy;
      if (value->kind == PTR_KIND) {
        VALUE argValue;
        void* argAddr = arg;
        argValue.as_ptr = argAddr;
        argCopy = new Location(PTR_KIND, argValue, true);
      } else {
        argCopy= new Location(arg->getType(), arg->getValue(), true);
      }
      callArgs.push(argCopy);
    }
  }

  // debugging
  printf("\n");

  callerVarIndex.push(inx);
  currentFrame[inx] = new Location(type, false);

  cout << currentFrame[inx]->toString() << "\n";
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
}
