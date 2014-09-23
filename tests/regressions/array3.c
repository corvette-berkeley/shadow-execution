int main() {
  //int p[5];
  int* p = (int *) malloc(5*sizeof(int));
  p[0] = 3;
  int **x = &p;

  printf("address of p %lld\n", (long long)p);
  printf("address of x %lld\n", (long long)x);
  printf("undefined value of x[0] %lld\n", (long long)x[0]);
}
