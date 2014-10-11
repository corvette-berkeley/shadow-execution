// Author: Cuong Nguyen

#ifndef BACKWARD_BLAME_ANALYSIS_H_
#define BACKWARD_BLAME_ANALYSIS_H_

#include "BlameTreeShadowObject.h"
#include "BlameTreeUtilities.h"
#include "BlameTree.h"
#include "BlameNodeID.h"
#include "../../src/Common.h"
#include "../../src/IValue.h"
#include "../../src/InterpreterObserver.h"
#include <math.h>
#include <fstream>

using namespace std;

class BackwardBlameAnalysis : public InterpreterObserver {

public:
	static int dpc;  // Unique counter for instructions executed.
	static map<uint64_t, DebugInfo> debugInfoMap;
	static vector<vector<BlameTreeShadowObject<HIGHPRECISION>>> trace;

	BackwardBlameAnalysis(std::string name) : InterpreterObserver(name) {}

	virtual void post_call_sin(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);
	virtual void post_call_acos(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);
	virtual void post_call_cos(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);
	virtual void post_call_fabs(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);
	virtual void post_call_sqrt(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);
	virtual void post_call_log(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);
	virtual void post_call_floor(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);
	virtual void post_call_exp(IID iid, IID argIID, SCOPE argScope, int64_t argVal, KIND type, int inx);

	virtual void post_fadd(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_fsub(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_fmul(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_fdiv(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue,
						   KIND type, int inx);

	virtual void post_fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

	virtual void post_fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

	virtual void pre_analysis();

	virtual void post_analysis();

private:
	/**
	   * Define how to copy BlameTreeShadowObject from the source IValue to
	   * the destination IValue.
	   */
	static void* copyShadow(void*);

	/**
	   * Return BlameTreeShadowObject associated with the given value.
	   *
	   * @note a value is denoted by a pair of scope and const/index.
	   * @param scope scope of the value.
	   * @param constOrIndex constant/index of the value.
	   * @return BlameTreeShadowObject associated with the given value.
	   */
	HIGHPRECISION getShadowValue(SCOPE scope, int64_t constOrIndex, PRECISION precision);

	/**
	   * Set BlameTreeShadowObject in corresponding IValue.
	   *
	   * @note a value is denoted by a pair of scope and const/index.
	   * @param scope scope of the value.
	   * @param constOrIndex constant/index of the value.
	   * @param shadowObject associated with the given value.
	   */
	void setShadowObject(SCOPE scope, int64_t inx, BlameTreeShadowObject<HIGHPRECISION>&& shadowObject);

	/**
	   * Return BlameTreeShadowObject associated with the given value.
	   *
	   * @note a value is denoted by a pair of scope and const/index.
	   * @param scope scope of the value.
	   * @return BlameTreeShadowObject associated with the given value.
	   */
	BlameTreeShadowObject<HIGHPRECISION>* getShadowObject(SCOPE scope, int64_t constOrIndex);

	/**
	   * Return the actual value in its lower precision.
	   *
	   * @note a value is denoted by a pair of scope and const/index.
	   * @param scope scope of the value.
	   * @param constOrIndex constant/index of the value.
	   * @return the actual value in its lower precision.
	   */
	LOWPRECISION getActualValue(SCOPE scope, int64_t constOrIndex);

	void post_fbinop(IID iid, IID liid, IID riid, SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type,
					 int inx, BINOP op);

	void post_lib_call(IID iid, IID argIID, SCOPE argScope, int64_t argValueOrIndex, KIND type, int inx, string func);
};

#endif
