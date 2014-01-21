
#include <stdlib.h>
#include <stdio.h> 

int main() {
  int* a = (int*) malloc(sizeof(int)*10);

  int i = 0;
  for(; i < 10; i++) {
    a[i] = i;
  }
  
  int result = a[3];

  long* z = (long*)a;
  long value = *(z+1);

  int* b = a + 2;
  int c = *b;
  
  
  short* d = (short*)a;
  
  short e = *(d+2);
  printf("E = %d\n", 5);

  short f = *(d+3);
  //interpreter_check_short(&e, 5); 
  d[5] = 4; // a short written in the middle of the third value

  int **g = &a;
  int h = (*g)[7];
  (*g)[5] = 77;

  printf("e: %d, f: %d", e, f);
  
  return 0;
}
