int main() {
  //int p[5]; 
  int* p = (int *) malloc(5*sizeof(int)); 
  p[0] = 3;
  int **x = &p;

  printf("%lld\n", (long long)p);
  printf("%lld\n", (long long)x);
  printf("%lld\n", (long long)x[0]);
}
