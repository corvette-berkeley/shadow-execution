#include <stdio.h>

int main() {
  double u, v, w;

  u = 2;
  v = -4; 

  for (int i = 3; i <= 100; i++)
  {
    w = 111. - 1130./v + 3000./(v*u);
    u = v;
    v = w;
    printf("u%d = %1.17g\n", i, v);
  }

  return 0;
}
