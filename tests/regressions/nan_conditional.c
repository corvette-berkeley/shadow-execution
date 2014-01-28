#include <stdio.h>

int main() {
  double a = 0;
  double b = 0;
  double c = a / b;
  printf("c: %lf\n", c);

  if (c != 0) { // we want to warn the user b is NaN!
    printf("Compute assuming x is non zero\n");
  }
  else {
    printf("Compute assuming x is zero\n");
  }

  return 0;
}

/*
  if (x .neq. 0)
     ... compute assuming x is nonzero
  else
     ... compute assuming x is zero
  end
  will not, since it will compute assuming x is zero */
