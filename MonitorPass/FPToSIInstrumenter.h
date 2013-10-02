/**
 * @file FPToSIInstrumenter.h
 * @brief FPToSIInstrumenter Declarations
 */

#ifndef FP_TO_SI_INSTRUMENTER_H_
#define FP_TO_SI_INSTRUMENTER_H_

#include "CastInstructionInstrumenter.h"

class FPToSIInstrumenter : public CastInstructionInstrumenter {
  public:
    FPToSIInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* FP_TO_SI_INSTRUMENTER_H_ */
