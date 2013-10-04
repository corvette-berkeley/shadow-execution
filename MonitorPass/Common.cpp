#include "Common.h"

#define UNRECOVERABLE_ERROR 5

void safe_assert(bool cond) {
  if (!cond) {
    printf("\nCounit: safe assert fail."); 
    printf(" \n\tfunction: %s\n\tfile: %s\n\tline: %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__); 
    fflush(stdout); 
    _Exit(UNRECOVERABLE_ERROR); 
  }
}

void unimpelemented() {
  printf(" \n Executing unimplemented code in function: %s\n\tfile: %s\n\tline: %d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__); 
  _Exit(UNRECOVERABLE_ERROR); 
}
