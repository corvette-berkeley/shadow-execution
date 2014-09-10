// Author: Cuong Nguyen

#ifndef BLAME_TREE_UTILITIES_H
#define BLAME_TREE_UTILITIES_H

#include "../src/Common.h"
#include "../src/Constants.h"
#include <inttypes.h>
#include <math.h>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

typedef double HIGHPRECISION;
typedef float LOWPRECISION;
typedef enum { 
  BITS_FLOAT, 
  /*
  BITS_8, BITS_9,
  BITS_10, BITS_11,
  */
  BITS_12, BITS_13,
  BITS_14, BITS_15,
  BITS_16, BITS_17, 
  BITS_18, 
  BITS_19, 
  BITS_20, BITS_21, 
  BITS_22, BITS_23,
  BITS_24, BITS_25, 
  BITS_26, 
  BITS_27, 
  BITS_28, BITS_29, 
  BITS_30, BITS_31,
  BITS_32, 
  BITS_33, 
  BITS_DOUBLE, PRECISION_NO 
} PRECISION;
typedef enum {
  BIN_INTR, UNARY_INTR, CONSTANT_INTR, CALL_INTR, LOAD_INTR, STORE_INTR, TRUNC_INTR, EXT_INTR,
  INTRTYPE_INVALID
} INTRTYPE;

class BlameTreeUtilities {
  public:
    static double clearBits(double v, int shift);

    static bool equalWithPrecision(double v1, double v2, PRECISION prec);

    /**
     * Evaluate the binary operation given the operator and two operands.
     *
     * @param value01 the first operand
     * @param value02 the second operand
     * @param bop the binary operator
     *
     * @return the result of the binary operation in HIGHPRECISION 
     */
    static HIGHPRECISION eval(HIGHPRECISION value01, HIGHPRECISION value02, BINOP bop);

    /**
     * Evaluate the binary operation given the operator and two operands.
     *
     * @param value01 the first operand
     * @param value02 the second operand
     * @param bop the binary operator
     *
     * @return the result of the binary operation in LOWPRECISION 
     */
    static LOWPRECISION feval(LOWPRECISION value01, LOWPRECISION value02, BINOP bop);

    /**
     * Evaluate the function operation given the function name and arguments.
     *
     * @param value01 the first argument
     * @param func the function name
     *
     * @return the result of the function operation in HIGHPRECISION
     */
    static HIGHPRECISION evalFunc(HIGHPRECISION value01, string func);

    /**
     * Convert from precision to a number specifying the exact bits.
     *
     * @param precision the precision to be converted
     *
     * @return the number of exact bits associated with the precision
     */
    static int exactBits(PRECISION precision);

    /**
     * Convert from the number of exact bits to precision.
     *
     * @param bit the number of exact bits
     *
     * @return the precision associated with the exact bits
     */
    static PRECISION exactBitToPrecision(int bit);

    /**
     * Convert precision to its string representation.
     *
     * @param precision the precision to be converted  
     *
     * @return the string representation of this precision
     */
    static string precisionToString(PRECISION precision);
};

#endif
