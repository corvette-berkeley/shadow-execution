int test(int* a) {
  int b  = *a;
  return b;
}

int main() {
  int** a;
  test(*a);

  return 0;
}
