#include <stdlib.h>
#include <stdio.h>

int main() {
  int* a = (int*) malloc(sizeof(int)*12);
  
  for(unsigned i = 0; i < 3; i++) {
    for (unsigned j = 0; j < 4; j++) {
      a[i][j] = i+j+1;
    }
  }

  int b = a[2][3];

  return b;
}
