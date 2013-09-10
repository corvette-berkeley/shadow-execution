
#include <stdio.h>
#include <math.h>

int main() {
  int x = 5;
  int y = 4;
  double z = 1.0;
  if (x*x-y*y < 2)
  {
    x = (int)z*3.0;
  }
  else
  {
    goto out;
  }

out:
  y = x * z;

  return y;
}
