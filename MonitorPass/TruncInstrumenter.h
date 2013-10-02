/**
 * @file TruncInstrumenter.h
 * @brief TruncInstrumenter Declarations
 */

#ifndef TRUNC_INSTRUMENTER_H_
#define TRUNC_INSTRUMENTER_H_

#include "CastInstructionInstrumenter.h"

class TruncInstrumenter : public CastInstructionInstrumenter {
  public: 
    TruncInstrumenter(std::string name, Instrumentation* instrumentation) :
    CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* TRUNC_INSTRUMENTER_H_ */
