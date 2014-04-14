#include <stdio.h>

int main() {
  double a[3][3];
  double l[3][3], u[3][3];
  int size = 3;

  a[0][0] = 1;
  a[0][1] = 3;
  a[0][2] = 5;
  
  a[1][0] = 2;
  a[1][1] = 4;
  a[1][2] = 7;
  
  a[2][0] = 1;
  a[2][1] = 1;
  a[2][2] = 0;

  for(int k=0;k<size;k++) {
    l[k][k]=1; // setting diagonal as 1?
    
    for(int j=k;j<size;j++) {
      long double sum=0;
      for(int s=0;s<=k-1;s++) {
	sum+= l[k][s]*u[s][j];
      }
      u[k][j]=a[k][j]-sum;
    }
    
    for(int i=k+1;i<size;i++) {
      long double sum=0;
      for(int s=0;s<=k-1;s++) {
	sum+=l[i][s]*u[s][k];
      }
      l[i][k]=(a[i][k]-sum)/u[k][k];
    }
    
  }

  printf("%lf\t%lf\t%lf\n", l[0][0], l[0][1], l[0][2]);
  printf("%lf\t%lf\t%lf\n", l[1][0], l[1][1], l[1][2]);
  printf("%lf\t%lf\t%lf\n", l[2][0], l[2][1], l[2][2]);    
  
  return 0;
}
