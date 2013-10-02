/**
 * @file StoreInstrumenter.h
 * @brief StoreInstrumenter Declarations.
 */

#ifndef STORE_INSTRUMENTER_H_
#define STORE_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class StoreInstrumenter : public Instrumenter {
  public:
    StoreInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* STORE_INSTRUMENTER_H_ */
