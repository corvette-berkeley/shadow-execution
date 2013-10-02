#ifndef BITCAST_INSTRUMENTER_H_
#define BITCAST_INSTRUMENTER_H_

#include "CastInstructionInstrumenter.h"

class BitCastInstrumenter : public CastInstructionInstrumenter {
  public:
    BitCastInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif
