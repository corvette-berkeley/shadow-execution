/**
 * @file IndirectBrInstrumenter.h
 * @brief IndirectBrInstrumenter Declarations.
 */

#ifndef INDIRECT_BR_INSTRUMENTER_H_
#define INDIRECT_BR_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class IndirectBrInstrumenter : public Instrumenter {
  public:
    IndirectBrInstrumenter(std::string name, Instrumentation* instrumentation) : Instrumenter(name, instrumentation) {};
    bool CheckAndInstrument(Instruction* inst);
};

#endif /* INDIRECT_BR_INSTRUMENTER_H_ */
