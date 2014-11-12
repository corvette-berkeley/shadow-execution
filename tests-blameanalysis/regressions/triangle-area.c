#include <stdio.h>

int main() {
  float a = 4.0f; 
  float b = 4.1f;
  float c = 8.0f;
  float d = 2.0f;

  float s = (a+b+c)/2.0f;

  float result = (s-a)*(s-b)*(s-c);

  return 0;
}
