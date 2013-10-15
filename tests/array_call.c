int sum_array(int n1, int n2, double arr[n1][n2]) {
  int sum = 0;
  int i, j;
  for (i = 0; i < n1; i++) {
    for (j = 0; j < n2; j++) {
      sum += arr[i][j]; 
    }
  }

  return sum;
}

int main() {
  double arr[3][4];
  int i, j;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      arr[i][j] = i*j;
    }
  }

  int sum = sum_array(3, 4, arr);

  return sum;
}
