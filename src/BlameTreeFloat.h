/**
 * @file BlameTreeFloat.h
 * @brief BlameTreeFloat Declarations.
 */

/*
 * Copyright (c) 2013, UC Berkeley All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1.  Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the UC Berkeley nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY UC BERKELEY ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL UC BERKELEY BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen and Cindy Rubio-Gonzalez

#ifndef BLAME_TREE_FLOAT_H_
#define BLAME_TREE_FLOAT_H_

#include <cmath>
#include <fstream>
#include "InterpreterObserver.h"
#include "BlameTreeShadowObject.h"
#include "BlameTreeUtilities.h"
#include "BlameTreeAnalysis.h"
#include "BlameNodeID.h"
#include "IValue.h"

class BlameTreeFloat : public InterpreterObserver {

public:
	static int dynamicCounter;  // unique counter for instructions executed
	static map<int, vector<BlameTreeShadowObject<HIGHPRECISION>>> trace;

	BlameTreeFloat(std::string name) : InterpreterObserver(name) {}

	virtual void post_call_sin(IID iid, bool nounwind, int pc, KIND type, int inx, SCOPE argScope,
							   int64_t argValueOrIndex);

	virtual void post_call_acos(IID iid, bool nounwind, int pc, KIND type, int inx, SCOPE argScope,
								int64_t argValueOrIndex);

	virtual void post_call_sqrt(IID iid, bool nounwind, int pc, KIND type, int inx, SCOPE argScope,
								int64_t argValueOrIndex);

	virtual void post_call_fabs(IID iid, bool nounwind, int pc, KIND type, int inx, SCOPE argScope,
								int64_t argValueOrIndex);

	virtual void post_call_cos(IID iid, bool nounwind, int pc, KIND type, int inx, SCOPE argScope,
							   int64_t argValueOrIndex);

	virtual void post_fadd(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line,
						   int col, int inx);

	virtual void post_fsub(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line,
						   int col, int inx);

	virtual void post_fmul(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line,
						   int col, int inx);

	virtual void post_fdiv(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line,
						   int col, int inx);

	virtual void post_fptrunc(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

	virtual void post_fpext(int64_t op, SCOPE opScope, KIND opKind, KIND kind, int size, int inx);

	virtual void post_create_global_symbol_table();

	virtual void post_analysis();

private:
	/**
	 * Define how to copy BlameTreeShadowObject from the source IValue to
	 * the destination IValue.
	 */
	static void copyShadow(IValue* src, IValue* dest);

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
	void setShadowObject(SCOPE scope, int64_t inx, BlameTreeShadowObject<HIGHPRECISION>* shadowObject);

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
	HIGHPRECISION getActualValue(SCOPE scope, int64_t constOrIndex);

	void post_fbinop(SCOPE lScope, SCOPE rScope, int64_t lValue, int64_t rValue, KIND type, int file, int line, int col,
					 int inx, BINOP op);
};

#endif
