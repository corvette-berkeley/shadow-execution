/**
 * @file Common.cpp
 * @brief
 */

#include "Common.h"
#include <stdio.h>

using namespace std;

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

std::string KVALUE_ToString(KVALUE* kv __attribute__((__unused__))) {

  std::stringstream s;

  if (kv) {
    s << "[IID: " << kv->iid << " ";  
    s << "[INX: " << kv->inx << " ";
    
    switch(kv->kind) {
    case PTR_KIND:
      s << "PTR: " << kv->value.as_ptr << "]";
      break;
    case INTPTR_KIND:
      s << "INTPTR: " <<  (int*)kv->value.as_ptr << " "<< kv->value.as_ptr << "]";
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
      // TODO: this assumes value.as_flp returns a long double value 
      s << "FLP32: " << (float) kv->value.as_flp << "]";
      break;
    case FLP64_KIND:
      // TODO: this assumes value.as_flp returns a long double value 
      s << "FLP64: " << (double) kv->value.as_flp << "]";
      break;
    case FLP80X86_KIND:
      // TODO: this assumes value.as_flp returns a long double value 
      s << "FLP80X86: " << kv->value.as_flp << "]";
      break;
    case ARRAY_KIND:
      s << "ARRAY: " << kv->value.as_flp << "]";
      break;
    case STRUCT_KIND:
      s << "STRUCT: " << kv->value.as_flp << "]";
      break;
    default: //safe_assert(false);
      break;
    }
    
    if (kv->isGlobal) {
      s << " GLOBAL";
    }
    
    s << "]";
  }
  else {
    printf("here\n");
    s << "Cindy";
    printf("after\n");
  }
  return s.str();

}

int64_t KVALUE_ToIntValue(KVALUE* kv) {
  int64_t v; 
  int64_t* vp64;
  int8_t* vp8;
  v = kv->value.as_int;
  switch (kv->kind) {
    case INT1_KIND:
      return (bool) v;
    case INT8_KIND:
      return (int8_t) v;
    case INT16_KIND:
      return (int16_t) v;
    case INT24_KIND:
      vp64 = &v;
      vp8 = (int8_t*) vp64;
      vp8[3] = 0;
      vp8[4] = 0;
      vp8[5] = 0;
      return *vp64;
    case INT32_KIND:
      return (int32_t) v;
    case INT64_KIND:
      return (int64_t) v;
    case INT80_KIND:
      cout << "[KVALUE_ToIntValue] Unsupported type INT80_KIND." << endl;
      safe_assert(false);
    default:
      return v;
  }

}
  
std::string KIND_ToString(int kind) {
  std::stringstream s;
  switch(kind) {
  case PTR_KIND:
    s << "[PTR]";
    break;
  case ARRAY_KIND:
    s << "[ARRAY]";
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
  case STRUCT_KIND:
    s << "[STRUCT]";
    break;
  case VOID_KIND:
    s << "[VOID]";
    break;
  default: //safe_assert(false);
    break;
  }
  return s.str();
}

int KIND_GetSize(int kind) {
  switch(kind) {
    case PTR_KIND:
      return sizeof(void*);
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
    case INT1_KIND:
      return 0;
//      return 1;
//      printf("Don't support bit right now!\n");
//      safe_assert(false);
    default:
      // shouldn't reach here
      safe_assert(false);
      return 0;
  }
}


