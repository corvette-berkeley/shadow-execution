#ifndef BITCAST_INSTRUMENTER_H_
#define BITCAST_INSTRUMENTER_H_

#include "CastInstructionInstrumenter.h"

class BitCastInstrumenter : public Instrumenter {
  public:
    BitCastInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};
};

#endif
