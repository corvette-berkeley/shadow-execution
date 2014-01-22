double addfun(double* a, double* b) 
{
  *a = 10;
  return *a + *b;
}

int main()
{
  double a = 4;
  double b = 5;
  double add, sub, mul, div, rem;
  add = addfun(&a, &b);
  sub = b - add;
  mul = add * a;
  div = mul / sub;

  return 0;
}
