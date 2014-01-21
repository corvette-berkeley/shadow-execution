typedef struct {
  int x;
  int y;
} point;

int foo(point a) {
  return a.x;
}

int foo2(int b, point a) {
  return b + a.x;
}

int main() {
  point p;
  p.x = 0;
  p.y = 3;
}
