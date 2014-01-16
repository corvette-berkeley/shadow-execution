
#ifndef INSTRUCTION_OBSERVER_H_
#define INSTRUCTION_OBSERVER_H_

#include "Common.h"
#define UNUSED __attribute__((__unused__))

// interface/adapter for observers
// all methods have default (empty) implementations, so no need to override all
class InstructionObserver {
public:
	InstructionObserver(std::string name) : name_(name) {}
	virtual ~InstructionObserver() {}

	// ***** Binary Operators **** //
	virtual void add(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void fadd(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void sub(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void fsub(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void mul(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void fmul(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void udiv(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void sdiv(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void fdiv(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void urem(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void srem(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void frem(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};

	// ***** Bitwise Binary Operators ***** //
	virtual void shl(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void lshr(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void ashr(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void and_(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void or_(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void xor_(IID iid UNUSED, bool nuw UNUSED, bool nsw UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};

	// ***** Vector Operations ***** //
	virtual void extractelement(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};
	virtual void insertelement() {};
	virtual void shufflevector() {};

	// ***** Aggregate Operations ***** //
	virtual void extractvalue(IID iid UNUSED, int inx UNUSED, int opinx UNUSED) {};
	virtual void insertvalue(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, int inx UNUSED) {};

	// ***** Memory Access and Addressing Operations ***** //
	virtual void allocax(IID iid UNUSED, KIND kind UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* result UNUSED) {};
	virtual void allocax_array(IID iid UNUSED, KIND kind UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* addr UNUSED) {};
	virtual void allocax_struct(IID iid UNUSED, uint64_t size UNUSED, int inx UNUSED, int line UNUSED, bool arg UNUSED, KVALUE* addr UNUSED) {};
	virtual void load(IID iid UNUSED, KIND kind UNUSED, KVALUE* op UNUSED, int line UNUSED, int inx UNUSED) {};
	virtual void store(IID iid UNUSED, KVALUE* op UNUSED, KVALUE* value UNUSED, int line UNUSED, int inx UNUSED) {};
	virtual void fence() {};
	virtual void cmpxchg(IID iid UNUSED, PTR addr UNUSED, KVALUE* value1 UNUSED, KVALUE* value2 UNUSED, int inx UNUSED) {};
	virtual void atomicrmw() {};
	virtual void getelementptr(IID iid UNUSED, bool inbound UNUSED, KVALUE* value UNUSED, KVALUE* index UNUSED, KIND kind UNUSED, 
				   uint64_t size UNUSED, int line UNUSED, int inx UNUSED) {};
  virtual void getelementptr_array(IID iid UNUSED, bool inbound UNUSED, KVALUE* value UNUSED, KIND kind UNUSED, int inx UNUSED) {};
  virtual void getelementptr_struct(IID iid UNUSED, bool inbound UNUSED, KVALUE* value UNUSED, KIND kind UNUSED, KIND arrayKind UNUSED, int inx UNUSED) {};

	// ***** Conversion Operations ***** //
	virtual void trunc(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void zext(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void sext(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void fptrunc(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void fpext(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void fptoui(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void fptosi(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void uitofp(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void sitofp(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void ptrtoint(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void inttoptr(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};
	virtual void bitcast(IID iid UNUSED, KIND type UNUSED, KVALUE* op UNUSED, uint64_t size UNUSED, int inx UNUSED) {};

	// ***** Terminator Instructions *****/
	virtual void branch(IID iid UNUSED, bool conditional UNUSED,  KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void branch2(IID iid UNUSED, bool conditional UNUSED, int inx UNUSED) {};
	virtual void indirectbr(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void invoke(IID iid UNUSED, KVALUE* op UNUSED, int inx UNUSED) {};
	virtual void resume(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void return_(IID iid UNUSED, KVALUE* op1 UNUSED, int inx UNUSED) {};
	virtual void return2_(IID iid UNUSED, int inx UNUSED) {};
	virtual void return_struct_(IID iid UNUSED, int inx UNUSED, int valInx UNUSED) {};
	virtual void switch_(IID iid UNUSED, KVALUE* op UNUSED, int inx UNUSED) {};
	virtual void unreachable() {};

	// ***** Other Operations ***** //
	virtual void icmp(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, PRED pred UNUSED, int inx UNUSED) {};
	virtual void fcmp(IID iid UNUSED, KVALUE* op1 UNUSED, KVALUE* op2 UNUSED, PRED pred UNUSED, int inx UNUSED) {};
	virtual void phinode(IID iid UNUSED, int inx UNUSED) {};
	virtual void select(IID iid UNUSED, KVALUE* cond UNUSED, KVALUE* tvalue UNUSED, KVALUE* fvealue UNUSED, int inx UNUSED) {};
	virtual void push_stack(KVALUE* value UNUSED) {};
	virtual void push_phinode_value(int valId UNUSED, int blockId UNUSED) {};
	virtual void push_phinode_constant_value(KVALUE* value UNUSED, int blockId UNUSED) {};
	virtual void push_return_struct(KVALUE* value UNUSED) {};
	virtual void push_struct_type(KIND kind UNUSED) {};
	virtual void push_getelementptr_inx(KVALUE* value UNUSED) {};
	virtual void push_getelementptr_inx2(int value UNUSED) {};
	virtual void push_array_size(uint64_t i UNUSED) {};
	virtual void construct_array_type(uint64_t i UNUSED) {};
	virtual void after_call(KVALUE* value UNUSED) {};
	virtual void after_void_call() {};
	virtual void after_struct_call() {};
	virtual void create_stack_frame(int size UNUSED) {};
	virtual void create_global_symbol_table(int size UNUSED) {};
	virtual void record_block_id(int id UNUSED) {};
	virtual void create_global(KVALUE* value UNUSED, KVALUE* initializer UNUSED) {};
	virtual void call(IID iid UNUSED, bool nounwind UNUSED, KIND type UNUSED, int inx UNUSED) {};
	virtual void call_malloc(IID iid UNUSED, bool nounwind UNUSED, KIND type UNUSED, KVALUE* value UNUSED, 
				 int size UNUSED, int inx UNUSED, KVALUE* mallocAddress UNUSED) {};
	virtual void vaarg() {};
	virtual void landingpad() {};

private:
	std::string name_;
};


#endif /* INSTRUCTIONOBSERVER_H_ */
