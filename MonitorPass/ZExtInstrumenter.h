/**
 * @file ZExtInstrumenter.h
 * @brief ZExtInstrumenter Declarations
 */

#ifndef Z_EXT_INSTRUMENTER_H_
#define Z_EXT_INSTRUMENTER_H_

#include "ZExtInstrumenter.h"

class ZExtInstrumenter : public CastInstructionInstrumenter {
  public: 
    ZExtInstrumenter(std::string name, Instrumentation* instrumentation) :
    CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* TRUNC_INSTRUMENTER_H_ */
