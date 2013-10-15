
#include <stdlib.h>
#include <stdio.h> 


struct X {
  int a;
  double b;
};

struct Y {
  int c;
};


int main() {
  
  struct X* x = (struct X*)malloc(sizeof(struct X) + sizeof(struct Y));
  x->a = 3;
  x->b = 5.5;
  struct Y* y = (struct Y*)(x+1);
  y->c = 6;

  return 0;
}
