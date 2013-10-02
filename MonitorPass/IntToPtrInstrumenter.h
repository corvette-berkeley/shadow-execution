/**
 * @file IntToPtrInstrumenter.h
 * @brief IntToPtrInstrumenter Declarations
 */

#ifndef INT_TO_PTR_INSTRUMENTER_H
#define INT_TO_PTR_INSTRUMENTER_H

#include "IntToPtrInstrumenter.h"

class IntToPtrInstrumenter : public CastInstructionInstrumenter {
  public:
    IntToPtrInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* INT_TO_PTR_INSTRUMENTER_H */
