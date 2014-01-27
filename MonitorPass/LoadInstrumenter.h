#ifndef LOAD_INSTRUMENTER_H_
#define LOAD_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class LoadInstrumenter : public Instrumenter {
  public:
    LoadInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    uint64_t pushStructType(StructType* structType, InstrPtrVector& instrs);
    uint64_t pushStructType(ArrayType* arrayType, InstrPtrVector& instrs);

    virtual bool CheckAndInstrument(Instruction* inst);
};

#endif
