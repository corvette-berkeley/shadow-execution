/**
 * @file Common.h
 * @brief Common Declarations.
 */

#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

// C/C++ libraries
#include <cstdlib>
#include <cstdio>
#include <dlfcn.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdint.h>
#include <semaphore.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <exception>
#include <iostream>
#include <sstream>

// std libraries
#include <string>
#include <vector>
#include <map>
#include <set>

// list of constants
#include "Constants.h"

typedef uintptr_t	ADDRINT;
typedef uint64_t 	IID;
typedef int64_t	 	INT;
typedef int32_t         INT32; 
typedef double	 	FLP;
typedef void*	 	PTR;
typedef bool            BOOL;

union value_t {
  INT as_int;
  FLP as_flp;
  PTR as_ptr;
};
#define VALUE value_t

typedef uint32_t pred_t;
#define PRED pred_t

typedef uint32_t kind_t;
#define KIND kind_t

const KIND	INV_KIND	= 0U,
  PTR_KIND 	= 1U,
  INT1_KIND 	= 2U,
  INT8_KIND 	= 3U,
  INT16_KIND 	= 4U,
  INT32_KIND 	= 5U,
  INT64_KIND 	= 6U,
  FLP32_KIND	= 7U,
  FLP64_KIND	= 8U,
  FLP128_KIND	= 9U,
  FLP80X86_KIND  = 10U,
  FLP128PPC_KIND = 11U,
  ARRAY_KIND = 12U,
  STRUCT_KIND = 13U,
  INTPTR_KIND = 14U,
  VOID_KIND = 15U;


const IID INV_IID = 0U;

#define KVALUE_ALIGNMENT	4

struct kvalue_t {
  IID	iid;
  INT32 inx;
  BOOL  isGlobal;
  KIND	kind;
  VALUE	value;
} __attribute__ ((__aligned__(KVALUE_ALIGNMENT)));
#define KVALUE kvalue_t

/********************************************************************************/

std::string IID_ToString(IID& iid);
std::string PTR_ToString(PTR& ptr);
std::string KVALUE_ToString(KVALUE& kv);
std::string KIND_ToString(int kind);
int KIND_GetSize(int kind);

#define UNRECOVERABLE_ERROR 5

#define safe_assert(cond) _safe_assert(cond, __PRETTY_FUNCTION__, __FILE__, __LINE__)  

inline void _safe_assert(bool cond, const char* func, std::string file, int line) {
  if (!cond) {
    printf("\nCounit: safe assert fail."); 
    printf(" \n\tfunction: %s\n\tfile: %s\n\tline: %d\n", func, file.c_str(), line); 
    fflush(stdout); 
    _Exit(UNRECOVERABLE_ERROR); 
  }
}

inline void unimplemented() {
  printf(" \n Executing unimplemented code in function: %s\n\tfile: %s\n\tline: %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__); 
  _Exit(UNRECOVERABLE_ERROR); 
}

template<typename T>
std::string Generic_ToString(T x) {
  std::stringstream s;
  s << x;
  return s.str();
}

/********************************************************************************/

#endif /* COMMON_DEFS_H_ */
