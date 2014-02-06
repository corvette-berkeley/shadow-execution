/**
 * Expected number of syncing: 2
 */

int main() {
  struct point {
    int x; 
    int y;
  };

  struct point p = {1, 2}; // using memcpy to initialize the struct here

  int i, a;

  a = p.x; // sync here
  a = p.y; // sync here
}
