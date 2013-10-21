
#include <stdlib.h>
#include <stdio.h> 

int main() {

  int a[5];

  for(int i = 0; i < 5; i++) {
    a[i] = i;
  }

  int result = a[3];
  a[4] = 5;

  return 0;
}
