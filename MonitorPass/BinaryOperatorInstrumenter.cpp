/**
 * @file BinaryOperatorInstrumenter.cpp
 * @brief
 */

#include "BinaryOperatorInstrumenter.h"

bool BinaryOperatorInstrumenter::CheckAndInstrument(Instruction* inst) {
	BinaryOperator* binInst;
	BINOP binop;
	BITWISE bitwise;

	binInst = dyn_cast<BinaryOperator>(inst);
	binop = getBinOp(binInst);
	bitwise = getBitWise(binInst);

	if (binInst != NULL && (binop != BINOP_INVALID || bitwise != BITWISE_INVALID)) {
		safe_assert(parent_ != NULL);

		count_++;

		InstrPtrVector instrs;
		Value* lValue, *rValue;
		Constant* cInx, *cType, *cLScope, *cRScope, *cLValue, *cRValue;

		lValue = binInst->getOperand(0);
		rValue = binInst->getOperand(1);

		cInx = computeIndex(binInst);
		cType = KIND_CONSTANT(TypeToKind(binInst->getType()));

		// debugging info
		string filename = getFileName(binInst);
		int line = getLineNumber(binInst);
		int column = getColumnNumber(binInst);

		DebugInfo* debug = new DebugInfo;
		sprintf(debug->file, "%.99s", filename.c_str());
		debug->line = line;
		debug->column = column;
		IID address = static_cast<IID>(reinterpret_cast<ADDRINT>(binInst));
		Constant* iid = IID_CONSTANT(binInst);
		parent_->debugMap[address] = debug;
		// end of debugging info

		if (parent_->fileNames.insert(std::make_pair(filename, parent_->fileCount)).second) {
			// element was inserted
			parent_->fileCount++;
		}

		cLScope = INT32_CONSTANT(getScope(lValue), SIGNED);
		cRScope = INT32_CONSTANT(getScope(rValue), SIGNED);

		cLValue = getValueOrIndex(lValue);
		cRValue = getValueOrIndex(rValue);

		Constant* cLIID = IID_CONSTANT(lValue);
		Constant* cRIID = IID_CONSTANT(rValue);

		std::stringstream callback;

		switch (binop) {
			case ADD:
				callback << "llvm_add";
				break;
			case SUB:
				callback << "llvm_sub";
				break;
			case MUL:
				callback << "llvm_mul";
				break;
			case UDIV:
				callback << "llvm_udiv";
				break;
			case SDIV:
				callback << "llvm_sdiv";
				break;
			case UREM:
				callback << "llvm_urem";
				break;
			case SREM:
				callback << "llvm_srem";
				break;
			case FADD:
				callback << "llvm_fadd";
				break;
			case FSUB:
				callback << "llvm_fsub";
				break;
			case FMUL:
				callback << "llvm_fmul";
				break;
			case FDIV:
				callback << "llvm_fdiv";
				break;
			case FREM:
				callback << "llvm_frem";
				break;
			case BINOP_INVALID:
				switch (bitwise) {
					case SHL:
						callback << "llvm_shl";
						break;
					case LSHR:
						callback << "llvm_lshr";
						break;
					case ASHR:
						callback << "llvm_ashr";
						break;
					case AND:
						callback << "llvm_and_";
						break;
					case OR:
						callback << "llvm_or_";
						break;
					case XOR:
						callback << "llvm_xor_";
						break;
					default:
						safe_assert(false);
						return false;  // this cannot happen
				}
				break;
			default:
				safe_assert(false);
				return false;  // this cannot happen
		}

		Instruction* call = CALL_IID_IID_IID_INT_INT_INT64_INT64_KIND_INT(callback.str().c_str(), iid, cLIID, cRIID,
							cLScope, cRScope, cLValue, cRValue, cType, cInx);

		instrs.push_back(call);

		// instrument
		InsertAllBefore(instrs, binInst);

		return true;
	} else {
		return false;
	}
}

BINOP BinaryOperatorInstrumenter::getBinOp(BinaryOperator* binInst) {
	if (binInst != NULL) {
		switch (binInst->getOpcode()) {
			case Instruction::BinaryOps(Instruction::Add) :
				return ADD;
			case Instruction::BinaryOps(Instruction::Sub) :
				return SUB;
			case Instruction::BinaryOps(Instruction::Mul) :
				return MUL;
			case Instruction::BinaryOps(Instruction::UDiv) :
				return UDIV;
			case Instruction::BinaryOps(Instruction::SDiv) :
				return SDIV;
			case Instruction::BinaryOps(Instruction::URem) :
				return UREM;
			case Instruction::BinaryOps(Instruction::SRem) :
				return SREM;
			case Instruction::BinaryOps(Instruction::FAdd) :
				return FADD;
			case Instruction::BinaryOps(Instruction::FSub) :
				return FSUB;
			case Instruction::BinaryOps(Instruction::FMul) :
				return FMUL;
			case Instruction::BinaryOps(Instruction::FDiv) :
				return FDIV;
			case Instruction::BinaryOps(Instruction::FRem) :
				return FREM;
			default:
				return BINOP_INVALID;
		}
	} else {
		return BINOP_INVALID;
	}
}

BITWISE BinaryOperatorInstrumenter::getBitWise(BinaryOperator* binInst) {
	if (binInst != NULL) {
		switch (binInst->getOpcode()) {
			case Instruction::BinaryOps(Instruction::Shl) :
				return SHL;
			case Instruction::BinaryOps(Instruction::LShr) :
				return LSHR;
			case Instruction::BinaryOps(Instruction::AShr) :
				return ASHR;
			case Instruction::BinaryOps(Instruction::And) :
				return AND;
			case Instruction::BinaryOps(Instruction::Or) :
				return OR;
			case Instruction::BinaryOps(Instruction::Xor) :
				return XOR;
			default:
				return BITWISE_INVALID;
		}
	} else {
		return BITWISE_INVALID;
	}
}
