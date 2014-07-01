

int ret_int() {
  return 4;
}

double ret_double() {
  return 4.5;
}



int main() {
  int a = 5;
  int c = ret_int() + a;
  int d = ret_double() + a;
  return 0;
}

