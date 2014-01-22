struct point {
  double a[2];
  int b;
} __attribute__((packed));

int main() {
  struct point p;
  int c;

  p.b = 5;
  c = p.b;

  return 0;
}
