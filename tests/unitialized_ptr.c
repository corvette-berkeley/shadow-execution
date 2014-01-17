
#include <stdio.h>

int main(int argv, char** argc) {

  char* temp = argc[1];
  char c = temp[1];
  printf("%c\n", c);
  
  return 0;
}
