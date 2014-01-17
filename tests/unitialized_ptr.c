
#include <stdio.h>

void f(char *name ) {
  interpreter_print(name, "unitialized");
  printf("ARG is %s\n", name);
}
int main(int argc, char** argv) {

  f(argv[0]);
  if(argc > 1)
    f(argv[1]);
  f(argv[argc-1];
  if (argv > 1) {
    temp = argc[1];
  } else {
    temp = argc[0];
  }
  
  return 0;
}
