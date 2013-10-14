typedef struct {
  int a;
  int b;
} point;

int main() {
  point a[2][3];
  int c, d;
  for (c = 0; c < 2; c++)
  {
    for (d = 0; d < 3; d++) {
      a[c][d].a = c*c + d;
      a[c][d].b = c+2+d;
    }
  }

  int e = a[1][2].b;

  return e;
}
