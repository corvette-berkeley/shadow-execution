
#include <stdlib.h>
#include <stdio.h> 

struct mystruct {
  long x;
  long y;
};


static struct mystruct foo(struct mystruct *instance) {
  instance->x = 5;
  instance->y = 6;
  long a = instance->x + instance->y;
  return *instance;
}

int main() {
  
  struct mystruct* mstruct = (struct mystruct*)malloc(sizeof(struct mystruct));
  mstruct->x = 1;
  mstruct->y = 2;

  struct mystruct di = foo(mstruct); // problematic
  printf("address of x: %p\n", &(di));
  printf("address of x: %p\n", &(di.x));
  printf("address of y: %p\n", &(di.y));

  return 0;
}
