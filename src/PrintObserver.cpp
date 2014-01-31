
#include "Common.h"
#include "PrintObserver.h"
#include <stack>

using namespace std;
	
void PrintObserver::load(IID iid, KIND type, KVALUE* op, int line, int inx) {
  DEBUG_STDOUT("<<<<< LOAD >>>>> " << IID_ToString(iid).c_str() << ", kind:" << KIND_ToString(type).c_str() << ", " << KVALUE_ToString(op).c_str() << ", line:" << line << ", [INX: " << inx << "]");
}

void PrintObserver::load_struct(IID iid, KIND type, KVALUE* op, int line, int inx) {
  DEBUG_STDOUT("<<<<< LOAD STRUCT >>>>> " << IID_ToString(iid).c_str() << ", kind:" << KIND_ToString(type).c_str() << ", " << KVALUE_ToString(op).c_str() << ", line:" << line << ", [INX: " << inx << "]");
}

// ***** Binary Operations ***** //

void PrintObserver::add(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< ADD >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::fadd(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< FADD >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::sub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< SUB >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::fsub(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< FSUB >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::mul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< MUL >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::fmul(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< FMUL >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::udiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< UDIV >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::sdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< SDIV >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::fdiv(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< FDIV >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::urem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< UREM >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::srem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< SREM >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::frem(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< FREM >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}


// ***** Bitwise Binary Operations ***** //
void PrintObserver::shl(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< SHL >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::lshr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< LSHR >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::ashr(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< ASHR >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::and_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< AND >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::or_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< OR >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

void PrintObserver::xor_(IID iid, bool nuw, bool nsw, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< XOR >>>>>" << IID_ToString(iid).c_str() << ", nuw: " << (nuw ? "1" : "0") << ", nsw: " << (nsw ? "1" : "0") << ", " << KVALUE_ToString(op1).c_str() << ", " << KVALUE_ToString(op2).c_str() << ", [INX: " << inx << "]");
}

// ***** Vector Operations ***** //

void PrintObserver::extractelement(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< EXTRACTELEMENT >>>>>" << IID_ToString(iid).c_str() << ", vector:" << KVALUE_ToString(op1).c_str() << ", index:" << KVALUE_ToString(op2) << ", [INX: " << inx << "]"); 
}

void PrintObserver::insertelement() {
  DEBUG_STDOUT("<<<<< INSERTELEMENT >>>>>");
}

void PrintObserver::shufflevector() {
  DEBUG_STDOUT("<<<<< SHUFFLEVECTOR >>>>>");
}


// ***** AGGREGATE OPERATIONS ***** //

void PrintObserver::extractvalue(IID iid, int inx, int opinx) {
  DEBUG_STDOUT("<<<<< EXTRACTVALUE >>>>> " << IID_ToString(iid).c_str() << ", agg_inx:" << opinx << ", [INX: " << inx << "]"); 
}

void PrintObserver::insertvalue(IID iid, KVALUE* op1, KVALUE* op2, int inx) {
  DEBUG_STDOUT("<<<<< INSERTVALUE >>>>>" << IID_ToString(iid).c_str() << ", vector:" << KVALUE_ToString(op1).c_str() << ", index:" << KVALUE_ToString(op2) << ", [INX: " << inx << "]"); 
}

// ***** Memory Access and Addressing Operations ***** //

void PrintObserver::allocax(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, KVALUE* result) {
  DEBUG_STDOUT("<<<<< ALLOCA >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", arg:" << arg << ", line:" << line << ", result:" << KVALUE_ToString(result) << ", [INX: " << inx << "]");
}

void PrintObserver::allocax_array(IID iid, KIND kind, uint64_t size, int inx, int line, bool arg, KVALUE* addr) {
  DEBUG_STDOUT("<<<<< ALLOCA >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", arg:" << arg << ", line:" << line << ", address:" << KVALUE_ToString(addr) << ", [INX: " << inx << "]");
}

void PrintObserver::allocax_struct(IID iid, uint64_t size, int inx, int line, bool arg, KVALUE* addr) {
  DEBUG_STDOUT("<<<<< ALLOCA >>>>> " << IID_ToString(iid) << ", size:" << size << ", arg:" << arg << ", line:" << line << ", address:" << KVALUE_ToString(addr) << ", [INX: " << inx << "]");
}

void PrintObserver::store(IID iid, KVALUE* op, KVALUE* kv, int file, int line, int inx) {
  DEBUG_STDOUT("<<<<< STORE >>>>> " << IID_ToString(iid) << ", source:" << KVALUE_ToString(op) << ", destination:" << KVALUE_ToString(kv) << ", file: " << file << ", line: " << line << ", [INX: " << inx << "]");
}

void PrintObserver::fence() {
  DEBUG_STDOUT("<<<<< FENCE >>>>>");
}

void PrintObserver::cmpxchg(IID iid, PTR addr, KVALUE* kv1, KVALUE* kv2, int inx) {
  DEBUG_STDOUT("<<<<< CMPXCHG >>>>> " << IID_ToString(iid) << ", address:" << PTR_ToString(addr) << ", operand 1:" << KVALUE_ToString(kv1) << ", operand 2:" << KVALUE_ToString(kv2) << ", [INX: " << inx << "]");
}

void PrintObserver::atomicrmw() {
  DEBUG_STDOUT("<<<<< ATOMICRMW >>>>>");
}

void PrintObserver::getelementptr(IID iid, bool inbound, KVALUE* op, KVALUE* index, KIND kind, uint64_t size, int line, int inx) {
  DEBUG_STDOUT("<<<<< GETELEMENTPTR >>>>> " << IID_ToString(iid) << ", inbound" << (inbound ? "1" : "0") << ", pointer value:" << KVALUE_ToString(op) << ", index:" << KVALUE_ToString(index) << ", kind:" << KIND_ToString(kind) << ", size:" << size << ", line:" << line << " [INX: " << inx << "]");
}

void PrintObserver::getelementptr_array(IID iid, bool inbound, KVALUE* op, KIND kind, int elementSize, int inx) {
  DEBUG_STDOUT("<<<<< GETELEMENTPTR ARRAY >>>>> " << IID_ToString(iid) << ", inbound" << (inbound ? "1" : "0") << ", pointer value:" << KVALUE_ToString(op) << ", elementSize:" << elementSize <<  ", kind:" << KIND_ToString(kind) << " [INX: " << inx << "]");
}

void PrintObserver::getelementptr_struct(IID iid, bool inbound, KVALUE* op, KIND kind, KIND arrayKind, int inx) {
  DEBUG_STDOUT("<<<<< GETELEMENTPTR STRUCT >>>>> " << IID_ToString(iid) << ", inbound" << (inbound ? "1" : "0") << ", pointer value:" << KVALUE_ToString(op) << ", kind:" << KIND_ToString(kind) << ", arrayKind:" << KIND_ToString(arrayKind) << " [INX: " << inx << "]");
}

// ***** Conversion Operations ***** //

void PrintObserver::trunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< TRUNC >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::zext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< ZEXT >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::sext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< SEXT >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::fptrunc(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< FPTRUNC >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::fpext(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< FPEXT >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::fptoui(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< FPTOUIT >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::fptosi(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< FPTOSI >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::uitofp(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< UITOFP >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::sitofp(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< SITOFP >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::ptrtoint(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< PTRTOINT >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::inttoptr(IID iid, KIND type, KVALUE* op, uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< INTOTPTR >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}

void PrintObserver::bitcast(IID iid, KIND type, KVALUE* op,  uint64_t size, int inx) {
  DEBUG_STDOUT("<<<<< BITCAST >>>>> " << IID_ToString(iid) << ", kind:" << KIND_ToString(type) << ", operand:" << KVALUE_ToString(op) << ", size:" << size << " [INX: " << inx << "]");
}


// ***** TerminatorInst ***** //
void PrintObserver::branch(IID iid, bool conditional, KVALUE* op1, int inx) {
  DEBUG_STDOUT("<<<<< BRANCH >>>>> " << IID_ToString(iid) << ", cond:" << (conditional ? "1" : "0") << ", condition value:" << KVALUE_ToString(op1) << " [INX: " << inx << "]");
}

void PrintObserver::branch2(IID iid, bool conditional, int inx) {
  DEBUG_STDOUT("<<<<< BRANCH [GOTO] >>>>> " << IID_ToString(iid) << ", cond:" << (conditional ? "1" : "0") << " [INX: " << inx << "]");
}

void PrintObserver::indirectbr(IID iid, KVALUE* op1, int inx) {
  DEBUG_STDOUT("<<<<< INDIRECTBR >>>>> " << IID_ToString(iid) << ", condition value:" << KVALUE_ToString(op1) << " [INX: " << inx << "]");
}

void PrintObserver::invoke(IID iid, KVALUE* call_value, int inx) {
  DEBUG_STDOUT("<<<<< INVOKE >>>>> " << IID_ToString(iid) << ", call value:" << KVALUE_ToString(call_value) << " [INX: " << inx << "]");

  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    DEBUG_STDOUT("\t arg: " << KVALUE_ToString(value));
    myStack.pop();
  }
  
}

void PrintObserver::resume(IID iid, KVALUE* op1, int inx) {
  DEBUG_STDOUT("<<<<< RESUME >>>>> " << IID_ToString(iid) << ", acc value:" << KVALUE_ToString(op1) << " [INX: " << inx << "]");
}

void PrintObserver::return_(IID iid, KVALUE* op1, int inx) {
  DEBUG_STDOUT("<<<<< RETURN >>>>> " << IID_ToString(iid) << ", return value:" << KVALUE_ToString(op1) << " [INX: " << inx << "]");
}

void PrintObserver::return2_(IID iid, int inx) {
  DEBUG_STDOUT("<<<<< RETURN VOID >>>>> " << IID_ToString(iid) << " [INX: " << inx << "]");
}

void PrintObserver::return_struct_(IID iid, int inx, int valInx) {
  DEBUG_STDOUT("<<<<< RETURN STRUCT >>>>> " << IID_ToString(iid) << ", index:" << valInx << " [INX: " << inx << "]");
}

void PrintObserver::switch_(IID iid, KVALUE* op, int inx) {
  DEBUG_STDOUT("<<<<< SWITCH >>>>> " << IID_ToString(iid) << ", condition:" << KVALUE_ToString(op) << " [INX: " << inx << "]");
}

void PrintObserver::unreachable() {
  DEBUG_STDOUT("<<<<< UNREACHABLE >>>>>");
}

// ***** Other Operations ***** //

void PrintObserver::icmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  DEBUG_STDOUT("<<<<< ICMP >>>>> " << IID_ToString(iid) << ", operand 1:" << KVALUE_ToString(op1) << ", operand 2:" << KVALUE_ToString(op2) << ", predicate:" << pred << " [INX: " << inx << "]");
}

void PrintObserver::fcmp(IID iid, KVALUE* op1, KVALUE* op2, PRED pred, int inx) {
  DEBUG_STDOUT("<<<<< FCMP >>>>> " << IID_ToString(iid) << ", operand 1:" << KVALUE_ToString(op1) << ", operand 2:" << KVALUE_ToString(op2) << ", predicate:" << pred << " [INX: " << inx << "]");
}

void PrintObserver::phinode(IID iid, int inx) {
  DEBUG_STDOUT("<<<<< PHINODE >>>>> " << IID_ToString(iid) << " [INX: " << inx << "]");
}

void PrintObserver::select(IID iid, KVALUE* cond, KVALUE* tvalue, KVALUE* fvalue, int inx) {
  DEBUG_STDOUT("<<<<< SELECT >>>>> " << IID_ToString(iid) << ", condition:" << KVALUE_ToString(cond) << ", true value:" << KVALUE_ToString(tvalue) << ", false value:" << KVALUE_ToString(fvalue) << " [INX: " << inx << "]");
}

void PrintObserver::push_stack(KVALUE* value) {
  DEBUG_STDOUT("<<<<< PUSH VALUE >>>>> value: " << KVALUE_ToString(value));
}

void PrintObserver::push_phinode_constant_value(KVALUE* value, int blockId) {
  DEBUG_STDOUT("<<<<< PUSH PHINODE CONSTANT VALUE >>>>> value: " << KVALUE_ToString(value) << ", block id:" << blockId);
}

void PrintObserver::push_phinode_value(int valId, int blockId) {
  DEBUG_STDOUT("<<<<< PUSH PHINODE VALUE >>>>> value id: " << valId << ", block id:" << blockId);
}

void PrintObserver::push_return_struct(KVALUE* value) {
  DEBUG_STDOUT("<<<<< PUSH RETURN STRUCT >>>>> value: " << KVALUE_ToString(value));
}

void PrintObserver::push_struct_type(KIND kind) {
  DEBUG_STDOUT("<<<<< PUSH STRUCT TYPE >>>>> kind: " << KIND_ToString(kind));
}

void PrintObserver::push_struct_element_size(uint64_t s) {
  DEBUG_STDOUT("<<<<< PUSH STRUCT ELEMENT SIZE >>>>> size: " << s);
}

void PrintObserver::push_getelementptr_inx(KVALUE* value) {
  DEBUG_STDOUT("<<<<< PUSH GETELEMENTPTR INX >>>>> value: " << KVALUE_ToString(value));
}

void PrintObserver::push_getelementptr_inx2(int value) {
  DEBUG_STDOUT("<<<<< PUSH GETELEMENTPTR INX  2>>>>> value: " << value);
}

void PrintObserver::push_array_size(uint64_t i) {
  DEBUG_STDOUT("<<<<< PUSH ARRAY SIZE >>>>> size: " << i);
}

void PrintObserver::construct_array_type(uint64_t i) {
  DEBUG_STDOUT("<<<<< CONSTRUCT ARRAY TYPE >>>>> size: " << i);
}

void PrintObserver::after_call(KVALUE* value) {
  DEBUG_STDOUT("<<<<< AFTER CALL >>>>> value: " << KVALUE_ToString(value));
}

void PrintObserver::after_void_call() {
  DEBUG_STDOUT("<<<<< AFTER VOID CALL >>>>>");
}

void PrintObserver::after_struct_call() {
  DEBUG_STDOUT("<<<<< AFTER STRUCT CALL >>>>>");
}

void PrintObserver::create_stack_frame(int size) {
  DEBUG_STDOUT("<<<<< CREATE STACK >>>>> size: " << size);
}

void PrintObserver::create_global_symbol_table(int size) {
  DEBUG_STDOUT("<<<<< CREATE SYMBOL TABLE >>>>> size: " << size);
}

void PrintObserver::record_block_id(int id) {
  DEBUG_STDOUT("<<<<< RECORD BLOCK ID >>>> id:" << id);
}

void PrintObserver::create_global(KVALUE* kvalue, KVALUE* initializer) {
  DEBUG_STDOUT("<<<<< CREATE GLOBAL >>>>> value:" << KVALUE_ToString(kvalue) << ", initializer:" << KVALUE_ToString(initializer));
}

void PrintObserver::call(IID iid, bool nounwind, KIND type, int inx) {
  DEBUG_STDOUT("<<<<< CALL >>>>> " << IID_ToString(iid) << ", nounwind:" << (nounwind ? 1 : 0) << ", return type:" << KIND_ToString(type) << " [INX: " << inx << "]");

  while (!myStack.empty()) {
    KVALUE* value = myStack.top();
    DEBUG_STDOUT("\t arg: " << KVALUE_ToString(value));
    myStack.pop();
  }
  
}

void PrintObserver::vaarg() {
  DEBUG_STDOUT("<<<<< VAARG >>>>>\n");
}

void PrintObserver::landingpad() {
  DEBUG_STDOUT("<<<<< LANDINGPAD >>>>>\n");
}

