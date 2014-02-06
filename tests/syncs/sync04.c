/**
 * Expected syncing: 5 
 */

int main() {
  int i, b;
  // using memcp here to initialize array which has side effect
  int a[5] = {1,2,3,4,5}; 

  for (i = 0; i < 5; i++) {
    b = a[i]; // sync here
    b = a[i] + 1; // no syncing here
  }
}
