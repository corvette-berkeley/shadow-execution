#include <math.h>
#include <stdio.h>

int main() {
  double a = NAN;
  double b = a;
  double c = b;
  
  if (b < c) {
    a = NAN;
    b = a;
    c = b;
  }
  else {
    b = NAN;
    c = b;
  }

  double d = c;

  c = 0; //?

  return 0;
}
