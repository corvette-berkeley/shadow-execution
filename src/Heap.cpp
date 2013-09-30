
#include "Heap.h"

/* malloc: general-purpose storage allocator */
void* Heap::Alloca(unsigned nbytes) {
  Header *p, *prevp;

  //Header *moreroce(unsigned); 
  unsigned nunits;
  nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
  cout << "Units needed" << nunits << "\n";

  if ((prevp = freep) == NULL) { /* no free list yet */
    base.s.ptr = freep = prevp = &base; // replaced prevptr -> prevp, freeptr -> freep  
    base.s.size = 0;
    cout << "It is the first block\n";
  }

  for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) { 

    cout << "Size of current block: " << p->s.size << "\n";

    if (p->s.size >= nunits) { /* big enough */
      cout << "\tIt is big enough\n";

      if (p->s.size == nunits) { /* exactly */ 
	cout << "It is exactly the required size\n";

	prevp->s.ptr = p->s.ptr;
      }
      else { /* allocate tail end */ 
	cout << "\tAllocating at the end\n";

	p->s.size -= nunits;
	p += p->s.size;
	p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p+1);
    }
    if (p == freep)  {/* wrapped around free list */
      cout << "There is no free block in list\n";

      if ((p = morecore(nunits)) == NULL) {
	cout << "\tThe space was not allocated";
	return NULL; /* none left */
      }
      else {
	cout << "\tSpace was allocated\n";
      }
    }
  }
}


void Heap::Free(void *ap) {
  Header *bp, *p;
  bp = (Header *)ap - 1; /* point to block header */ 
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr)) 
      break; /* freed block at start or end of arena */
  if (bp + bp->s.size == p->s.ptr) { 
    bp->s.size += p->s.ptr->s.size; bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) { 
    p->s.size += bp->s.size; p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p; 
}


Header* Heap::morecore(unsigned nu) {
  char *cp;
  Header *up;

  if (nu < NALLOC) {
    nu = NALLOC;
  }

  cp = (char*)sbrk(nu * sizeof(Header));

  if (cp == (char *) -1) { /* no space at all */
    return NULL;
  }

  up = (Header *) cp; 
  up->s.size = nu; 
  cout << "Here in morecore\n";
  Free((void *)(up+1)); 
  return freep;
}

