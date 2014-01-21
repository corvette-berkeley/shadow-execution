int fun(int* a) {
  int b = *a;

  return b;
}

int main() {
  int* a;
  int b;

  b = 5;
  a = &b;
  b = fun(a);

  return 0;
}
