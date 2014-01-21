int foo(int a, int b) {
  return a + b;
}

int main() {
  int (*bar)(int, int);
  bar = &foo;

  bar(1, 2);

  return 0;
}
