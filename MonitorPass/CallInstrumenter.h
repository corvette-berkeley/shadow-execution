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
};

#endif

