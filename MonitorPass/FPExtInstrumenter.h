/**
 * @file FPExtInstrumenter.h
 * @brief FPExtInstrumenter Declarations
 */

#ifndef FPEXT_INSTRUMENTER_H
#define FPEXT_INSTRUMENTER_H

#include "CastInstructionInstrumenter.h"

class FPExtInstrumenter : public CastInstructionInstrumenter {
  public:
    FPExtInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* TRUNC_INSTRUMENTER_H_ */
