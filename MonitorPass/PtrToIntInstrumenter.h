/**
 * @file PtrToIntInstrumenter.h
 * @brief PtrToIntInstrumenter Declarations
 */

#ifndef PTR_TO_INT_INSTRUMENTER_H
#define PTR_TO_INT_INSTRUMENTER_H

#include "PtrToIntInstrumenter.h"

class PtrToIntInstrumenter : public CastInstructionInstrumenter {
  public: 
    PtrToIntInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* PTR_TO_INT_INSTRUMENTER_H */
