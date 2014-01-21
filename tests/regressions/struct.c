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

  printf("Address of struct p is: %ld\n", &p);
  printf("Address of the first field in p is: %ld\n", &p.x);

  return 0;
}
