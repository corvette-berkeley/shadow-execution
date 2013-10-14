typedef struct {
  int a;
  int b;
} point;

int main() {
  point a[2];
  int c;
  for (c = 0; c < 2; c++)
  {
    a[c].a = c*c;
    a[c].b = c+2;
  }

  int d = a[1].b;

  return d;
}
