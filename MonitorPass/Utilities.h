#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "Common.h"

class Utilities 
{
  public:
    template<class T> static bool cast(Instruction* inst, T* tInst);
    template<class T> static bool castBinary(BinaryOperator*
        binInst, Instruction* inst, Instruction::BinaryOps op);
};

#endif /* UTILITIES_H_ */
