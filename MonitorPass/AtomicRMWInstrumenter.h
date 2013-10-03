/**
 * @file AtomicRMWInstrumenter.h
 * @brief AtomicRMWInstrumenter Declarations
 */

#ifndef ATOMIC_RMW_INSTRUMENTER_H_
#define ATOMIC_RMW_INSTRUMENTER_H_

#include "UninterpretedInstrumenter.h"

class AtomicRMWInstrumenter : public UninterpretedInstrumenter {
  public:
    AtomicRMWInstrumenter(std::string name, Instrumentation* instrumentation) :
      UninterpretedInstrumenter(name, instrumentation) {}; 
};

#endif /* ATOMIC_RMW_INSTRUMENTER_H_ */
