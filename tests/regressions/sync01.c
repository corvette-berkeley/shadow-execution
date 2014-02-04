#include <stdio.h>
#include <stdlib.h>

/*
 * Expected number of syncs: 9
 * Expected number of static syncs: 1
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
    b = a[i]; // no syncing when i = 0 because the value is initialized as 0.
    b = a[i]; // no syncing because the value is synced previously
  }
}
