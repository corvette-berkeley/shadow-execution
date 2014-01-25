
#include <stdio.h>
#include <stdlib.h>

struct arguments {
  int x;
  union {
    signed char                 a_schar;
    unsigned char               a_uchar;
    short                       a_short;
    unsigned short              a_ushort;
    int                         a_int;
    unsigned int                a_uint;
    long int                    a_longint;
    unsigned long int           a_ulongint;
    float                       a_float;
    double                      a_double;
    long double                 a_longdouble;
    int                         a_char;
    const char*                 a_string;
    void*                       a_pointer;
    signed char *               a_count_schar_pointer;
    short *                     a_count_short_pointer;
    int *                       a_count_int_pointer;
    long int *                  a_count_longint_pointer;
  } y;
} __attribute__((packed)) ;


int main() {

  printf("size of union %ld\n", sizeof(union {
    signed char                 a_schar;
    unsigned char               a_uchar;
    short                       a_short;
    unsigned short              a_ushort;
    int                         a_int;
    unsigned int                a_uint;
    long int                    a_longint;
    unsigned long int           a_ulongint;
    float                       a_float;
    double                      a_double;
    long double                 a_longdouble;
    int                         a_char;
    const char*                 a_string;
    void*                       a_pointer;
    signed char *               a_count_schar_pointer;
    short *                     a_count_short_pointer;
    int *                       a_count_int_pointer;
    long int *                  a_count_longint_pointer;
  }));

  printf("size of long double %ld\n", sizeof (long double));

  struct arguments* args = (struct arguments*)malloc(sizeof(struct arguments)*10);
  args = args + 1;

  return 0;
}
