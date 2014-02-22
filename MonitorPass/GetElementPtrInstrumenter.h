/**
 * @file GetElementPtrInstrumenter.h
 * @brief GetElementPtrInstrumenter Declarations.
 */

#ifndef GET_ELEMENT_PTR_INSTRUMENTER_H_
#define GET_ELEMENT_PTR_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class GetElementPtrInstrumenter : public Instrumenter {
  typedef struct {
    SCOPE scope;
    Constant *valOrInx;
  } ActualValue;

  private:
    ActualValue getActualValue(Value *value);

  public:
    GetElementPtrInstrumenter(std::string name, Instrumentation* instrumentation) : Instrumenter(name, instrumentation){};
    bool CheckAndInstrument(Instruction* inst);
    uint64_t pushStructType(StructType* structType, InstrPtrVector& instrs);
    uint64_t pushStructType(ArrayType* arrayType, InstrPtrVector& instrs);
    uint64_t getFlatLength(StructType* structType);
    uint64_t getFlatLength(ArrayType* arrayType); 
    uint64_t getFlatSize(StructType* structType);
    uint64_t getFlatSize(ArrayType* arrayType); 
    void pushStructElementSize(StructType* structType, InstrPtrVector& instrs);
};

#endif
