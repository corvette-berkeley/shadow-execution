/**
 * @file Common.cpp
 * @brief Define common functions used accross all observers.
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
 * 3. All advertising materials mentioning features or use of this software must
 * display the following acknowledgement: This product includes software
 * developed by the UC Berkeley.
 *
 * 4. Neither the name of the UC Berkeley nor the names of its contributors may
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

#include "Common.h"

#include <stdio.h>

std::string IID_ToString(IID& iid) {
	std::stringstream s;
	s << "[IID: " << iid << "]";
	return s.str();
}

std::string PTR_ToString(PTR& ptr) {
	std::stringstream s;
	s << "[PTR: " << ptr << "]";
	return s.str();
}

std::string SCOPE_ToString(SCOPE scope) {
  
  std::stringstream s;

  switch (scope) {
    case GLOBAL:
      s << "GLOBAL";
      break;
    case LOCAL:
      s << "LOCAL";
      break;
    case CONSTANT:
      s << "CONSTANT";
      break;
    case REGISTER:
      s << "REGISTER";
      break;
    default:
      DEBUG_STDERR("Unsupport scope: " << scope); 
      safe_assert(false);
  }

  return s.str();
}

std::string KVALUE_ToString(KVALUE* kv) {

  std::stringstream s;

  if (kv) {
    s << "[INX: " << kv->inx << " ";
    
    switch(kv->kind) {
      case PTR_KIND:
        s << "PTR: " << kv->value.as_ptr << "]";
        break;
      case INT1_KIND:
        s << "INT1: " << kv->value.as_int << "]";
        break;
      case INT8_KIND:
        s << "INT8: " << kv->value.as_int << "]";
        break;
      case INT16_KIND:
        s << "INT16: " << kv->value.as_int << "]";
        break;
      case INT24_KIND:
        s << "INT24: " << kv->value.as_int << "]";
        break;
      case INT32_KIND:
        s << "INT32: " << kv->value.as_int << "]";
        break;
      case INT64_KIND:
        s << "INT64: " << kv->value.as_int << "]";
        break;
      case INT80_KIND:
        s << "INT80: " << kv->value.as_int << "]";
        break;
      case FLP32_KIND:
        s << "FLP32: " << kv->value.as_flp << "]";
        break;
      case FLP64_KIND:
        s << "FLP64: " << kv->value.as_flp << "]";
        break;
      case FLP80X86_KIND:
        s << "FLP80X86: " << kv->value.as_flp << "]";
        break;
      case ARRAY_KIND:
        s << "ARRAY: " << kv->value.as_flp << "]";
        break;
      case STRUCT_KIND:
        s << "STRUCT: " << kv->value.as_flp << "]";
        break;
      default: 
        safe_assert(false);
        break;
    }
    
    if (kv->isGlobal) {
      s << " GLOBAL";
    }
    
    s << "]";
  } else {
    DEBUG_STDERR("KVALUE is NULL.");
    safe_assert(false);
  }
  return s.str();

}

int64_t KVALUE_ToIntValue(KVALUE* kv) {
  int32_t v32;
  int32_t *v32Ptr;
  int64_t v64;
  int64_t *v64Ptr;

  //
  // truncating 64-bit integer value to 32-bit integer value
  //
  v64 = kv->value.as_int;
  v64Ptr = &v64;
  v32Ptr = (int32_t*) v64Ptr;
  v32 = *v32Ptr;

  //
  // returning integer value depending on type
  //
  switch (kv->kind) {
    case INT1_KIND:
      return v32 & 0x1;;
    case INT8_KIND:
      return v32 & 0x000000FF;
    case INT16_KIND:
      return v32 & 0x0000FFFF;
    case INT24_KIND:
      return v32 & 0x00FFFFFF; 
    case INT32_KIND:
      return v32;
    case INT64_KIND:
      return v64;
    case INT80_KIND:
      DEBUG_STDERR("Unsupported type INT80_KIND.");
      safe_assert(false);
      return v64;
    default:
      return v64;
  }

}
  
uint64_t KVALUE_ToUIntValue(KVALUE* kv) {
  int64_t v64;

  v64 = KVALUE_ToIntValue(kv);

  //
  // returning unsigned integer value depending on type
  //
  switch (kv->kind) {
    case INT1_KIND:
      return v64;
    case INT8_KIND:
      return (uint8_t) v64;
    case INT16_KIND:
      return (uint16_t) v64;
    case INT24_KIND:
    case INT32_KIND:
      return (uint32_t) v64;
    case INT64_KIND:
      return (uint64_t) v64;
    case INT80_KIND:
      DEBUG_STDERR("Unsupported type INT80_KIND.");
      safe_assert(false);
      return v64;
    default:
      return v64;
  }

}
  
long double KVALUE_ToFlpValue(KVALUE* kv) {
  double v64;

  v64 = kv->value.as_flp;

  //
  // returning float value depending on type
  //
  switch (kv->kind) {
    case FLP32_KIND:
      return (float) v64;
    case FLP64_KIND:
      return v64;
    case FLP80X86_KIND:
      return v64;
    case FLP128_KIND:
    case FLP128PPC_KIND:
      DEBUG_STDERR("Unsupported float type: " << kv->kind);
      safe_assert(false);
      return v64;
    default:
      return v64;
  }

}

bool KVALUE_IsFlpValue(KVALUE *kv) {
  unsigned kind = kv->kind;
  return kind == FLP32_KIND || kind == FLP64_KIND || FLP80X86_KIND
    || FLP128_KIND || FLP128PPC_KIND;
}

bool KVALUE_IsIntValue(KVALUE *kv) {
  unsigned kind = kv->kind;
  return kind == INT1_KIND || INT8_KIND || INT16_KIND || INT24_KIND
    || INT32_KIND || INT64_KIND || INT80_KIND;
}

bool KVALUE_IsPtrValue(KVALUE *kv) {
  unsigned kind = kv->kind;
  return kind == PTR_KIND;
}

std::string KIND_ToString(int kind) {
  std::stringstream s;
  switch(kind) {
    case PTR_KIND:
      s << "[PTR]";
      break;
    case INT1_KIND:
      s << "[INT1]";
      break;
    case INT8_KIND:
      s << "[INT8]";
      break;
    case INT16_KIND:
      s << "[INT16]";
      break;
    case INT24_KIND:
      s << "[INT24]";
      break;
    case INT32_KIND:
      s << "[INT32]";
      break;
    case INT64_KIND:
      s << "[INT64]";
      break;
    case INT80_KIND:
      s << "[INT80]";
      break;
    case FLP32_KIND:
      s << "[FLP32]";
      break;
    case FLP64_KIND:
      s << "[FLP64]";
      break;
    case FLP80X86_KIND:
      s << "[FLP80X86]";
      break;
    case ARRAY_KIND:
      s << "[ARRAY]";
      break;
    case STRUCT_KIND:
      s << "[STRUCT]";
      break;
    case VOID_KIND:
      s << "[VOID]";
      break;
    default: 
      DEBUG_STDERR("Unsupported kind value " << kind);
      safe_assert(false);
      break;
  }
  return s.str();
}

int KIND_GetSize(int kind) {
  switch(kind) {
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


