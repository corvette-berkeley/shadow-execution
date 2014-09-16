// Author: Cuong Nguyen

#ifndef BLAME_TREE_SHADOW_OBJECT_H
#define BLAME_TREE_SHADOW_OBJECT_H

#include "BlameTreeUtilities.h"
#include "../src/Constants.h"
#include <limits>
#include <sstream>

/**
 * BlameTreeShadowObject is an abstraction of a value in the program in
 * higher precision.
 *
 * We make an assumption that each value corresponds to an instruction in the
 * program (similar to the way LLVM program constructed). Therefore, each
 * BlameTreeShadowObject also has attributes like intruction type, program
 * location, etc.
 *
 * Each BlameTreeShadowObject is more informative than the actual value in
 * higher precision because it consists of other information for error
 * analysis, e.g. maximum relative errors, sources of maximum relative errors,
 * etc.
 */

template <class T> class BlameTreeShadowObject {

private:
	string file;           // Id of the file containing this instruction.
	int pc;                // Program counter of the instruction associate
	// with this object.
	int col;               // Column offset of the instruction associate with
	// this object.
	int dpc;               // Program counter of the instruction as appeared
	// in the execution trace.
	INTRTYPE intrType;     // Type of the instruction.
	BINOP binOp;           // Binary operator (if instruction is BINOP).
	string func;           // Name of the function call (if instruction is CALL).
	T value[PRECISION_NO]; // Value in different precisions.

public:
	BlameTreeShadowObject()
		: file(""), pc(0), col(0), dpc(0), intrType(INTRTYPE_INVALID),
		  binOp(BINOP_INVALID), func("NONE") {
		PRECISION i;
		for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
			value[i] = 0;
		}
	}
	;

	BlameTreeShadowObject(string fl, int p, int c, int dp, INTRTYPE it, BINOP bo,
						  string f, T* val)
		: file(fl), pc(p), col(c), dpc(dp), intrType(it), binOp(bo), func(f) {
		PRECISION i;
		for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
			value[i] = val[i];
		}
	}

	BlameTreeShadowObject(const BlameTreeShadowObject& btmSO) {
		create(btmSO);
	}
	;

	~BlameTreeShadowObject() {
		uncreate();
	}

	BlameTreeShadowObject& operator=(const BlameTreeShadowObject& btmSO) {
		if (&btmSO != this) {

			// free the object in the left-hand side
			uncreate();

			// copy elements from the righ-hand side to the left-hand side
			create(btmSO);
		}

		return *this;
	}
	;

	int getPC() const {
		return pc;
	}
	;

	void setPC(int pc) {
		this->pc = pc;
	}
	;

	int getCol() const {
		return col;
	}
	;

	void setCol(int col) {
		this->col = col;
	}
	;

	int getDPC() const {
		return dpc;
	}
	;

	void setDPC(int dpc) {
		this->dpc = dpc;
	}
	;

	string getFile() const {
		return file;
	}
	;

	void setFile(string file) {
		this->file = file;
	}
	;

	INTRTYPE getIntrType() const {
		return intrType;
	}
	;

	void setIntrType(INTRTYPE intrType) {
		this->intrType = intrType;
	}
	;

	BINOP getBinOp() const {
		return binOp;
	}
	;

	void setBinOp(BINOP binOp) {
		this->binOp = binOp;
	}
	;

	string getFunc() const {
		return func;
	}
	;

	void setFunc(string func) {
		this->func = func;
	}
	;

	T getValue(int i) const {
		return value[i];
	}
	;

	void setValue(int i, T v) {
		value[i] = v;
	}
	;

	std::string BINOP_ToString(int binop) {
		std::stringstream s;
		switch (binop) {
			case ADD:
				s << "ADD";
				break;
			case FADD:
				s << "FADD";
				break;
			case SUB:
				s << "SUB";
				break;
			case FSUB:
				s << "FSUB";
				break;
			case MUL:
				s << "MUL";
				break;
			case FMUL:
				s << "FMUL";
				break;
			case UDIV:
				s << "UDIV";
				break;
			case SDIV:
				s << "SDIV";
				break;
			case FDIV:
				s << "FDIV";
				break;
			case UREM:
				s << "UREM";
				break;
			case SREM:
				s << "SREM";
				break;
			case FREM:
				s << "FREM";
				break;
			default:
				s << "INVALID";
				break;
		}
		return s.str();
	}
	;

	void print() {
		PRECISION i;

		cout << "[SHADOW]<pc: " << pc << ", col: " << col << ", dpc: " << dpc;
		cout.precision(10);
		for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
			cout << ", " << BlameTreeUtilities::precisionToString(i) << ":"
				 << value[i];
		}
		cout << ", op: " << BINOP_ToString(binOp).c_str()
			 << ", func:" << func.c_str() << ", file: " << file << endl;
	}

private:
	void create(const BlameTreeShadowObject& btmSO) {
		PRECISION i;

		pc = btmSO.getPC();
		col = btmSO.getCol();
		dpc = btmSO.getDPC();
		file = btmSO.getFile();
		intrType = btmSO.getIntrType();
		binOp = btmSO.getBinOp();
		func = btmSO.getFunc();
		for (i = BITS_FLOAT; i < PRECISION_NO; i = PRECISION(i + 1)) {
			value[i] = btmSO.getValue(i);
		}
	}
	;

	void uncreate() {}
	;
};

#endif
