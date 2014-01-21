struct point {
  long a;
  long b;
};

//point p;

static struct point foo() {
  struct point pp;
  pp.a = 3;
  pp.b = 4;
  return pp;
}

int main() {
  struct point p;
  p.a = 1;
  p.b = 2;
  p = foo();
  int sum = p.a + p.b;
  return 0;
}
