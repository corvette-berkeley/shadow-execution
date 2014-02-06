/**
 * Expected number of syncs: 27
 */

int main() {
  char* a;
  char c;
  int i;

  a = "this is a length 28 string.";

  for (i = 0; i < 28; i++) {
    c = a[i]; // sync here
    c = a[i];
  }

}
