int main() {
  int a[3][2];
  int c, d, e;

  for (c = 1; c < 3; c++) {
    for  (d= 1; d < 2; d++) {
      a[c][d] = c*d;
      e = a[c][d];
    }
  }

  return e;
}
