#include <stdlib.h>

int main() {
  long* a = (long*) malloc(5*sizeof(long));
  a[0] = 2L;
  a[3] = 4L;
  long b = a[3];

  return b;
}
