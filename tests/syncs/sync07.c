#include <stdio.h>
#include <stdlib.h>

/*
 * Expected number of syncs: 0
 * Expected number of static syncs: 0
 */

/**
 * This function is not instrumented.
 * See sync07-include.txt for list of all instrumented functions.
 */
void initialize(int* a, int size) {
  int i;

  for (i = 0; i < size; i++) {
    a[i] = i;
  }
}

int main() {
  int* a = (int*) malloc(10*sizeof(int));
  int b, i;

  initialize(a, 10);

  for (i = 0; i < 10; i++) {
    a[i] = 2;
    b = a[i]; // no syncing 
    b = a[i]; // no syncing 
  }
}
