/**
 * @file Common.h
 * @brief Common Declarations.
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
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

// Author: Cuong Nguyen and Cindy Rubio-Gonzalez

#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

#include <cstdlib>
#include <cstdio>
#include <string>
#include <exception>
#include <iostream>
#include <sstream>

#include <string>
#include <vector>
#include <map>
#include <set>

#include "Constants.h"

using std::cout;
using std::endl;

//
// Macros for debugging mode
//
//#define DEBUG_BUILD 1

#ifdef DEBUG_BUILD
#define DEBUG(x) x
#define DEBUG_STDERR(x) std::cerr << x << endl
#define DEBUG_STDOUT(x) std::cout << x << endl
#define DEBUG_LOG(x) LOG(INFO) << x << endl
#else
#define DEBUG(x)                                                                                                       \
	while (0) {                                                                                                          \
		x                                                                                                                  \
	}
#define DEBUG_STDERR(x)                                                                                                \
	while (0) {                                                                                                          \
		std::cerr << x << endl;                                                                                            \
	}
#define DEBUG_STDOUT(x)                                                                                                \
	while (0) {                                                                                                          \
		std::cerr << x << endl;                                                                                            \
	}
#define DEBUG_LOG(x)                                                                                                   \
	while (0) {                                                                                                          \
		std::cerr << x << endl;                                                                                            \
	}
//#define DEBUG_LOG(x) LOG(INFO) << x << endl
#endif

//
// Give meaningful names to types
//
typedef uintptr_t ADDRINT;
typedef uint64_t IID;
typedef int64_t INT;
typedef int32_t INT32;
typedef double FLP;
typedef void* PTR;
typedef bool BOOL;

//
// Data structures used in the observer
//
union value_t {
	INT as_int;
	FLP as_flp;
	PTR as_ptr;

	value_t(INT a = 0) : as_int(a) {}
	explicit value_t(PTR a) : as_ptr(a) {}
	explicit value_t(FLP a) : as_flp(a) {}
};
#define VALUE value_t

typedef uint32_t pred_t;
#define PRED pred_t

typedef uint32_t kind_t;
#define KIND kind_t

const KIND INV_KIND = 0U, PTR_KIND = 1U, INT1_KIND = 2U, INT8_KIND = 3U, INT16_KIND = 4U, INT24_KIND = 5U,
		   INT32_KIND = 6U, INT64_KIND = 7U, INT80_KIND = 8U, FLP32_KIND = 9U, FLP64_KIND = 10U, FLP128_KIND = 11U,
		   FLP80X86_KIND = 12U, FLP128PPC_KIND = 13U, ARRAY_KIND = 14U, STRUCT_KIND = 15U, VOID_KIND = 16U;

const IID INV_IID = 0U;

#define KVALUE_ALIGNMENT 4

struct kvalue_t {
	INT32 inx;
	BOOL isGlobal;
	KIND kind;
	VALUE value;
} __attribute__((__aligned__(KVALUE_ALIGNMENT)));
#define KVALUE kvalue_t

struct DebugInfo {
	int line;
	int column;
	char file[100];
};

//
// Inlined error functions
//
#define UNRECOVERABLE_ERROR 5

#define safe_assert(cond) _safe_assert(cond, __PRETTY_FUNCTION__, __FILE__, __LINE__)

//
// Definitions of commonly used functions
//

/**
 * Assert a condition and terminate the program in case of fail assertion.
 *
 * @param cond the condition to be checked
 * @param func the current function
 * @param file the current file
 * @param line the current line
 */
inline void _safe_assert(bool cond, const char* func, const char* file, int line) {
	if (!cond) {
		cout << "Counit: safe assert fail." << endl;
		cout << "\tfunction: " << func << "\tfile: " << file << "\tline: " << line << endl;
		fflush(stdout);
		// TODO: Why are we using _Exit?  is it really that important
		// that we abandon all state and exit instead of exit safely?
		_Exit(UNRECOVERABLE_ERROR);
	}
}

/**
 * Printing error messages on unimplemented code and terminate the program.
 */
[[noreturn]] inline void unimplemented() {
	cout << "Executing unimplemented code in function: " << __PRETTY_FUNCTION__ << endl;
	cout << "\tfile: " << __FILE__ << endl;
	cout << "\tline: " << __LINE__ << endl;
	// TODO: Why are we using _Exit?  is it really that important
	// that we abandon all state and exit instead of exit safely?
	_Exit(UNRECOVERABLE_ERROR);
}

/**
 * Convert iid to a string.
 *
 * @param iid the idd to be converted.
 * @return string representation of the idd.
 */
std::string IID_ToString(IID& iid);

/**
 * Convert a pointer value to a string.
 *
 * @param ptr the pointer value to be converted.
 * @return string representation of the pointer value.
 */
std::string PTR_ToString(PTR& ptr);

/**
 * Pretty printing for KVALUE.
 *
 * @param kv the kvalue to be printed.
 * @return string representation of kvalue kv.
 */
std::string KVALUE_ToString(KVALUE* kv);

std::string SCOPE_ToString(SCOPE sc);

/**
 * Return int value for KVALUE depending on its type.
 *
 * @param kv the kvalue to get integer value from.
 * @return integer value of kv.
 */
int64_t KVALUE_ToIntValue(KVALUE* kv);

/**
 * Return unsigned int value for KVALUE depending on its type.
 *
 * @param kv the kvalue to get integer value from.
 * @return unsigned integer value of kv.
 */
uint64_t KVALUE_ToUIntValue(KVALUE* kv);

/**
 * Return float value for KVALUE depending on its type.
 *
 * @param kv the kvalue to get integer value from.
 * @return float value of kv.
 */
long double KVALUE_ToFlpValue(KVALUE* kv);

/**
 * Return whether KVALUE is of floating-point type.
 *
 * @param kv the kvalue under check
 * @return true if kv has floating-point type; false otherwise.
 */
bool KVALUE_IsFlpValue(KVALUE* kv);

/**
 * Return whether KVALUE is of integer type.
 *
 * @param kv the kvalue under check
 * @return true if kv has integer type; false otherwise.
 */
bool KVALUE_IsIntValue(KVALUE* kv);

/**
 * Return whether KVALUE is of pointer type.
 *
 * @param kv the kvalue under check
 * @return true if kv has pointer type; false otherwise.
 */
bool KVALUE_IsPtrValue(KVALUE* kv);

/**
 * Pretty printing for KIND.
 *
 * @param kind the kind to be printed.
 * @return string reprensetation of kind.
 */
std::string KIND_ToString(KIND kind);

/**
 * Get size of kind.
 *
 * @param kind the kind to get size from.
 * @return size of kind.
 */
inline unsigned KIND_GetSize(KIND kind) {
	switch (kind) {
		case PTR_KIND:
			return sizeof(void*);
		case INT1_KIND:
			return 0;
		case INT8_KIND:
			return 1;
		case INT16_KIND:
			return 2;
		case INT24_KIND:
			return 3;
		case INT32_KIND:
		case FLP32_KIND:
			return 4;
		case INT64_KIND:
		case FLP64_KIND:
			return 8;
		case INT80_KIND:
			return 10;
		case FLP128_KIND:
			return 16;
		case FLP80X86_KIND:
			return 16;
		case FLP128PPC_KIND:
			return 16;
		default:
			DEBUG_STDERR("Unsupported kind value " << kind);
			safe_assert(false);
			return 0;
	}
}
#endif /* COMMON_DEFS_H_ */
