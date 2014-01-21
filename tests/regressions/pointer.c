int main() {
  int a = 5;
  int* b = &a;
  int** c = &b;
  int d = **c;

  return d;
}
