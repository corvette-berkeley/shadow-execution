#include <stdlib.h>
#include <stdio.h>

int main() {
  int* a = (int*) malloc(sizeof(int)*3);
  int b = *(a+2);
  b = 5;

  return *(a+2);
}
