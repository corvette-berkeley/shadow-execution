/**
 * @file SIToFPInstrumenter.h
 * @brief SIToFPInstrumenter Declarations
 */

#ifndef SI_TO_FP_INSTRUMENTER_H_
#define SI_TO_FP_INSTRUMENTER_H_

#include "SIToFPInstrumenter.h"

class SIToFPInstrumenter : public CastInstructionInstrumenter {
  public:
    SIToFPInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* SI_TO_FP_INSTRUMENTER_H */
