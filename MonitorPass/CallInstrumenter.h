/**
 * @file CallInstrumenter.h
 * @brief CallInstrumenter Declarations.
 */

#ifndef CALL_INSTRUMENTER_H_
#define CALL_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"


class CallInstrumenter : public Instrumenter {
  public:
    CallInstrumenter(std::string name, Instrumentation* instrumentation) : Instrumenter(name, instrumentation) {};
    bool CheckAndInstrument(Instruction* I);
  private:
    uint64_t pushStructType(StructType* structType, InstrPtrVector& instrs);
    uint64_t pushStructType(ArrayType* arrayType, InstrPtrVector& instrs);
    unsigned getFlatSize(ArrayType* arrayType);
    unsigned getFlatSize(StructType* structType);
};

#endif

