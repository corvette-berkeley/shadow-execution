#ifndef HEAP_H_
#define HEAP_H_

#include <stdlib.h>
#include <iostream>

using namespace std;

typedef long Align; /* for alignment to long boundary */

union header { /* block header */ 
  struct {
    union header *ptr; /* next block if on free list */
    unsigned size; /* size of this block */ 
  } s;
  Align x; /* force alignment of blocks */ 
};

typedef union header Header;
#define NALLOC 1024 /* minimum #units to request */


class Heap {

 private:
  static Header base; /* empty list to get started */ 
  static Header *freep; /* start of free list */

 public:
  Heap() {
    freep = NULL;
  }

  void* Alloca(unsigned int nbytes);

  void Free(void *ap);

 private:
  Header *morecore(unsigned int nu);

};

#endif /* HEAP_H_ */
