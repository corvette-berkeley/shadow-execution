typedef struct {
  int x;
  int y;
} point;

int main() {
  point p;
  int a;
  p.x = 0;
  p.y = 3;
  a = p.x + p.y;

  return a;
}
