
#include <stdlib.h>
#include <stdio.h> 



struct Y {
  int x;
  double y;
  int z;
} __attribute__((packed));

struct X {
  int a;
  struct Y y;
  int d;
  double b;
  char name[1];
  char* ptrname;
} __attribute__((packed));


int main() {
  
  struct X* x = (struct X*)malloc(sizeof(struct X));
  x->a = 1;
  x->y.x = 2;
  x->y.y = 3.3;
  x->y.z = 4;
  x->d = 5;
  x->b = 2.1;
  x->ptrname = x->name; // FAIL
  double c = x->b;

  return 0;
}
