#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include <cassert>
#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"

using namespace std;

using namespace llvm;

Function* getFunction(string fname, FunctionType* ftype, Instruction* instr) {
	return dyn_cast<Function>(instr->getParent()->getParent()->getParent()->getOrInsertFunction(fname, ftype));
}

Constant* getIID(Value* v) {
	static unsigned id = 0;
	static unordered_map<Value*, unsigned> encountered;
	if (encountered.find(v) == encountered.end()) {
		encountered[v] = id++;
	}
	return ConstantInt::get(Type::getInt32Ty(v->getContext()), encountered[v]);
}

Value* castToDouble(Value* v, Instruction* i) {
	if (v->getType()->isDoubleTy()) {
		return v;
	}
	auto ci = CastInst::CreateFPCast(v, Type::getDoubleTy(v->getContext()));
	if (v != i) {
		ci->insertBefore(i);
	} else {
		ci->insertAfter(i);
	}
	return ci;
}

CastInst* castToVoid(Value* v, Instruction* i) {
	auto ci = CastInst::CreatePointerCast(v, PointerType::get(Type::getVoidTy(v->getContext()), 0));
	if (v != i) {
		ci->insertBefore(i);
	} else {
		ci->insertAfter(i);
	}
	return ci;
}

bool useful(BinaryOperator* bi) {
	switch (bi->getOpcode()) {
		case Instruction::BinaryOps(Instruction::FAdd) :
		case Instruction::BinaryOps(Instruction::FSub) :
		case Instruction::BinaryOps(Instruction::FMul) :
		case Instruction::BinaryOps(Instruction::FDiv) :
		case Instruction::BinaryOps(Instruction::FRem) :
			return true;
		default:
			return false;
	}
}

string to_function_name(BinaryOperator* bi) {
	switch (bi->getOpcode()) {
		case Instruction::BinaryOps(Instruction::FAdd) :
			return "llvm_fadd";
		case Instruction::BinaryOps(Instruction::FSub) :
			return "llvm_fsub";
		case Instruction::BinaryOps(Instruction::FMul) :
			return "llvm_fmul";
		case Instruction::BinaryOps(Instruction::FDiv) :
			return "llvm_fdiv";
		case Instruction::BinaryOps(Instruction::FRem) :
			return "llvm_frem";
		default:
			return "";
	}
}

FunctionType* to_function_type(BinaryOperator* instr) {
	LLVMContext& cx = instr->getContext();
	return FunctionType::get(Type::getVoidTy(cx),
							 vector<Type*>({Type::getInt32Ty(cx),  Type::getDoubleTy(cx), Type::getInt32Ty(cx),
											Type::getDoubleTy(cx), Type::getInt32Ty(cx),  Type::getDoubleTy(cx),
										   }),
							 false);
}

void _handle(BinaryOperator* bin_instr, Function* f) {
	auto iidl = getIID(bin_instr->getOperand(0));
	auto iidr = getIID(bin_instr->getOperand(1));
	auto iid = getIID(bin_instr);

	Instruction* last = dyn_cast<Instruction>(castToDouble(bin_instr, bin_instr));
	assert(last);

	vector<Value*> args = {iid,  last,
						   iidl, castToDouble(bin_instr->getOperand(0), bin_instr),
						   iidr, castToDouble(bin_instr->getOperand(1), bin_instr)
						  };

	CallInst* ci = llvm::CallInst::Create(f, args);
	ci->insertAfter(last);
}

bool useful(CallInst* ci) {
	const set<string> possible = {"fabs", "exp", "sqrt", "log", "sin", "acos", "cos", "floor", };
	if (ci->getCalledFunction() == nullptr) {
		return false;
	}
	return possible.find(ci->getCalledFunction()->getName()) != possible.end();
}

string to_function_name(CallInst* ci) {
	return "llvm_call_" + string(ci->getCalledFunction()->getName());
}

FunctionType* to_function_type(CallInst* instr) {
	LLVMContext& cx = instr->getContext();
	return FunctionType::get(Type::getVoidTy(cx), vector<Type*>({Type::getInt32Ty(cx), Type::getDoubleTy(cx),
							 Type::getInt32Ty(cx), Type::getDoubleTy(cx),
																}),
							 false);
}

void _handle(CallInst* call_inst, Function* f) {

	auto iidl = getIID(call_inst->getArgOperand(0));
	auto iid = getIID(call_inst);

	Instruction* last = dyn_cast<Instruction>(castToDouble(call_inst, call_inst));
	assert(last);

	vector<Value*> args = {iid, last, iidl, castToDouble(call_inst->getArgOperand(0), call_inst), };

	CallInst* ci = llvm::CallInst::Create(f, args);
	ci->insertAfter(last);
}

bool useful(LoadInst* li) {
	return li->getPointerOperand()->getType()->getPointerElementType()->isFloatingPointTy();
}

string to_function_name(LoadInst*) {
	return "llvm_fload";
}

FunctionType* to_function_type(LoadInst* instr) {
	LLVMContext& cx = instr->getContext();
	return FunctionType::get(Type::getVoidTy(cx),
							 vector<Type*>({Type::getInt32Ty(cx), Type::getDoubleTy(cx),
											Type::getInt32Ty(cx), PointerType::get(Type::getVoidTy(cx), 0)
										   }),
							 false);
}

void _handle(LoadInst* load_inst, Function* f) {
	auto iidl = getIID(load_inst->getPointerOperand());
	auto iid = getIID(load_inst);

	Instruction* last = dyn_cast<Instruction>(castToDouble(load_inst, load_inst));
	assert(last);

	vector<Value*> args = {iid, last, iidl, castToVoid(load_inst->getPointerOperand(), load_inst)};

	CallInst* ci = llvm::CallInst::Create(f, args);
	ci->insertAfter(last);
}

bool useful(StoreInst* si) {
	return si->getPointerOperand()->getType()->getPointerElementType()->isFloatingPointTy();
}

string to_function_name(StoreInst*) {
	return "llvm_fstore";
}

FunctionType* to_function_type(StoreInst* instr) {
	LLVMContext& cx = instr->getContext();
	return FunctionType::get(Type::getVoidTy(cx),
							 vector<Type*>({Type::getInt32Ty(cx), Type::getDoubleTy(cx),
											Type::getInt32Ty(cx), PointerType::get(Type::getVoidTy(cx), 0)
										   }),
							 false);
}

void _handle(StoreInst* store_inst, Function* f) {
	auto iid = getIID(store_inst->getValueOperand());
	auto iidl = getIID(store_inst->getPointerOperand());

	vector<Value*> args = {iid,  castToDouble(store_inst->getValueOperand(), store_inst),
						   iidl, castToVoid(store_inst->getPointerOperand(), store_inst)
						  };

	CallInst* ci = llvm::CallInst::Create(f, args);
	ci->insertAfter(store_inst);
}

bool useful(PHINode* pi) {
	return pi->getType()->isFloatingPointTy();
}

string to_function_name(PHINode*) {
	return "llvm_fphi";
}

FunctionType* to_function_type(PHINode* instr) {
	LLVMContext& cx = instr->getContext();
	return FunctionType::get(Type::getVoidTy(cx),
							 vector<Type*>({Type::getInt32Ty(cx), Type::getDoubleTy(cx), Type::getInt32Ty(cx)}), false);
}

void _handle(PHINode* phi_inst, Function* f) {
	PHINode* phi_iid = PHINode::Create(Type::getInt32Ty(phi_inst->getContext()), phi_inst->getNumIncomingValues());
	assert(phi_iid->getNumIncomingValues() == 0);
	for (int i = 0; i < phi_inst->getNumIncomingValues(); ++i) {
		phi_iid->addIncoming(getIID(phi_inst->getIncomingValue(i)), phi_inst->getIncomingBlock(i));
	}
	phi_iid->insertAfter(phi_inst);
	assert(!phi_iid->getType()->isFloatingPointTy());

	auto iid = getIID(phi_inst);

	Instruction* last = dyn_cast<Instruction>(castToDouble(phi_inst, phi_inst->getParent()->getFirstInsertionPt()));
	assert(last);

	vector<Value*> args = {iid, last, phi_iid};

	CallInst* ci = llvm::CallInst::Create(f, args);
	ci->insertBefore(phi_inst->getParent()->getFirstInsertionPt());
}

vector<function<void()>> todo;

template <typename T> bool handle(Instruction* inst) {
	T* value = dyn_cast<T>(inst);
	if (!value) {
		return false;
	}

	if (useful(value)) {
		todo.push_back([value]() {
			// these really should be in one function, but C++ makes multiple return values painful
			Function* f = getFunction(to_function_name(value), to_function_type(value), value);
			_handle(value, f);
		});
	}
	return true;
}

void handleAll() {
	for (auto fn : todo) {
		fn();
	}
	todo.clear();
}

namespace {

struct FPPass : public BasicBlockPass {
	static char ID;
	FPPass() : BasicBlockPass(ID) {}
	using Pass::doInitialization;
	using Pass::doFinalization;
	bool doInitialization(Function&) {
		return true;
	}
	bool doFinalization(Function&) {
		return true;
	}

	bool runOnBasicBlock(BasicBlock& BB) {
		bool ret = false;
		for (Instruction& inst : BB) {
			if (handle<BinaryOperator>(&inst) || handle<CallInst>(&inst) || handle<LoadInst>(&inst) ||
					handle<StoreInst>(&inst) || handle<PHINode>(&inst)) {
				ret = true;
			}
		}
		if (ret) {
			handleAll();
		}
		return ret;
	}
};
}

char FPPass::ID = 0;
static RegisterPass<FPPass> X("fppass", "FLP pass", false, false);
