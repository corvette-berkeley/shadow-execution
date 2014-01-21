#include "stdio.h"
#include <stdbool.h>

static bool
foo (int a, int options)
{
  bool user_proc = true;
  if (user_proc)
    return false;
  if (a == 1 && options == 2)
    return false;
  return true;
}


int main() {
  bool r = foo(3,4);
  return 0;
}
