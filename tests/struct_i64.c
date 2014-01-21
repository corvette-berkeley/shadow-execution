
#include <stdlib.h>
#include <stdio.h> 

struct mystruct {
  long x;
  long y;
};


static struct mystruct foo(struct mystruct *instance) {

  return *instance;
}

int main() {
  
  struct mystruct* mstruct = (struct mystruct*)malloc(sizeof(struct mystruct));
  mstruct->x = 5;
  mstruct->y = 7;

  struct mystruct di = foo(mstruct);

  return 0;
}
