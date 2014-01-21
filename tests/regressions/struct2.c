typedef struct {
  int coor[2];
} point;

typedef struct {
  point a;
  point b;
} line;

int main() {
  point p1, p2;
  line l;
  p1.coor[0] = 0;
  p1.coor[1] = 3;
  p2.coor[0] = 5;
  p2.coor[1] = 6;
  l.a = p1;
  l.b = p2;

  int result = l.a.coor[0] + l.b.coor[1];
  return 0;
}

