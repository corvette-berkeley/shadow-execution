/**
 * @file SExtInstrumenter.h
 * @brief SExtInstrumenter Declarations
 */

#ifndef S_EXT_INSTRUMENTER_H_
#define S_EXT_INSTRUMENTER_H_

#include "SExtInstrumenter.h"

class SExtInstrumenter : public CastInstructionInstrumenter {
  public: 
    SExtInstrumenter(std::string name, Instrumentation* instrumentation) :
    CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* TRUNC_INSTRUMENTER_H_ */
