#include "Utilities.h"

template<class T>
bool Utilities::cast(Instruction* inst, T* tInst)
{
  tInst = dyn_cast<T>(inst);
  if (tInst == NULL) {
    return false;
  } else {
    return true;
  }
}

template<class T>
bool Utilities::castBinary(BinaryOperator* binInst, Instruction*
    inst, Instruction::BinaryOps op)
{
  return true;
}
