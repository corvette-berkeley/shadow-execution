double addfun(double* a, double* b) 
{
  return *a + *b;
}

int main()
{
  double a = 4;
  double b = 5;

  addfun(&a,&b);

  return 0;
}
