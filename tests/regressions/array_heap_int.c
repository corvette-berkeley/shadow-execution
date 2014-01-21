
#include <stdlib.h>
#include <stdio.h> 

int main() {
  int* array = (int*) malloc(5*sizeof(int));

  int i = 0;
  for(; i < 5; i++) {
    array[i] = i;
    //printf("An element: %d\n", array[i]);
  }

  int result = array[3];
  //printf("El result is: %d\n", result);

  return 0;
}
