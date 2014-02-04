#include <stdio.h>
#include <stdlib.h>

/**
 * Expected number of syncing: argc*2+2
 * Expected number of static syncing: 2 
 */

int main(int argc, char** argv) {
  char c; 
  int i;

  for (i = 0; i < argc; i++) { // sync argc here
    // one sync for argv for the first time too
    c = *argv[i]; // 2 syncs here (one for argv[i] and one for *argv[i])
    c = *argv[i]; // no syncs here, previously synced
  }
}
