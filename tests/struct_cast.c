#include <stdlib.h>

struct point {
  int x;
  int y;
};

int main() {
  int* a = (int*) malloc(4*sizeof(int));
  int b;
  a[0] = 1;
  a[1] = 2;
  a[2] = 3;
  a[3] = 4;
  struct point* p = (struct point*) a;

  b = p[1].x;

  return 0;
}
