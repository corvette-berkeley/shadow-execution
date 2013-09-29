/**
 * @file GetElementPtrInstrumenter.h
 * @brief GetElementPtrInstrumenter Declarations.
 */

#ifndef GET_ELEMENT_PTR_INSTRUMENTER_H_
#define GET_ELEMENT_PTR_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class GetElementPtrInstrumenter : public Instrumenter {
  public:
    GetElementPtrInstrumenter(std::string name, Instrumentation* instrumentation) : Instrumenter(name, instrumentation){};
    bool CheckAndInstrument(Instruction* inst);
};

#endif
