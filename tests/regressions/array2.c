int main() {
  int p[5]; 
  p[0] = 3;
  int **x = &p;
  int d = x[0];

  return d;
}
