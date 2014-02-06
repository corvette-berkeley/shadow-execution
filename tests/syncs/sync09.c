/**
 * Expected number of syncs: 3
 */

char* foo() {
  return "abc";
}

int main() {
  char* a;
  char c;
  int i;

  // we just construct an empty pointer IValue here because we do not know its size 
  a = foo(); 

  for (i = 0; i < 3; i++) {
    c = a[i]; // three syncs here
  }
}
