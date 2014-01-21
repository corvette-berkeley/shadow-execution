struct point {
  double a;
  int b;
} __attribute__((packed));

int main() {
  struct point a[2];
  int* b;

  b = &a[1];

  return 0;
}
