int or(int a, int b) {
  int c = a || b;

  return c;
}

int main() {
  int a = 3*3;
  int b = 4*4;
  int c = 5*5;

  int d = or(a + b > c, a + b < c);

  return d;
}

