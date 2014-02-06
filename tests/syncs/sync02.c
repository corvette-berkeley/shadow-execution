#include <stdio.h>
#include <stdlib.h>

/**
 * Expected number of syncs: 10
 * Expected number of static syncs: 1
 */

/**
 * This function is not instrumented.
 * See sync02-include.txt for list of all instrumented functions.
 */
void initialize(int* a, int size) {
  int i;

  for (i = 0; i < size; i++) {
    a[i] = i+1;
  }
}

int main() {
  int* a = (int*) malloc(10*sizeof(int));
  int b, i;

  initialize(a, 10);

  for (i = 0; i < 10; i++) {
    b = a[i]; // sync here
    b = a[i]; // no sync because the value is previously synced
  }
}
