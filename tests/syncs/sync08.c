/**
 * Expected number of syncs: 0
 */

struct point {
  int a; 
  int b;
} p;

int x = 1; 
int y = 2;

int main() {
  p.a = x; // no sync here, global var should be initialized
  p.b = y; // no sync here, global var should be initialized

  return 0;
}
