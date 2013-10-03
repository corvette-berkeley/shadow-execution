/**
 * @file AtomicCmpXchgInstrumenter.h
 * @brief
 */

#ifndef ATOMIC_CMP_XCHG_INSTRUMENTER_H_
#define ATOMIC_CMP_XCHG_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class AtomicCmpXchgInstrumenter : public Instrumenter {
  public:
    AtomicCmpXchgInstrumenter(std::string name, Instrumentation* instrumentation)
      : Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* ATOMIC_CMP_XCHG_INSTRUMENTER_H_ */
