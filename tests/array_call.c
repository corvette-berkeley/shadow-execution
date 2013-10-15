int sum_array(int n1, int n2, double arr[n1]) {
  int sum = 0;
  int i, j;
  for (i = 0; i < n1; i++) {
    sum += arr[i]; 
  }

  return sum;
}

int main() {
  double arr[3];
  int i, j;
  for (i = 0; i < 3; i++) {
    arr[i] = i;
  }

  int sum = sum_array(3, 4, arr);

  return sum;
}
