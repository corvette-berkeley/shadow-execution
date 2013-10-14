/**
 * @file AllocaInstrumenter.h
 * @brief
 */

#ifndef ALLOCA_INSTRUMENTER_H_
#define ALLOCA_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class AllocaInstrumenter : public Instrumenter {
  public:
    AllocaInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};
    bool CheckAndInstrument(Instruction* I);
  private:
    uint64_t pushStructType(StructType* structType, InstrPtrVector& instrs);
    uint64_t pushStructType(ArrayType* arrayType, InstrPtrVector& instrs);
};

#endif /* ALLOCA_INSTRUMENTER_H_ */
