int foo(int a, int b) {
  return a + b;
}

int main() {
  int (*bar)(int, int);
  bar = &foo;

  return bar(1, 2);
}
