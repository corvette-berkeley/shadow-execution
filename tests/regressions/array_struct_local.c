typedef struct {
  double a;
  int b;
} point;

int main() {
  point a[2];
  int c, d;
  int * e;

  for (c = 0; c < 2; c++)
  {
    a[c].a = c*c;
    a[c].b = c+2;
  }

  e = &a[0];
  e = &a[1];

  d = a[0].a;
  d = a[0].b;
  d = a[1].a;
  d = a[1].b;

  return 0;
}
