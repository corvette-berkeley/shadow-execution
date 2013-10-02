/**
 * @file UnreachableInstrumenter.h
 * @brief
 */

#ifndef UNREACHABLE_INSTRUMENTER_H_
#define UNREACHABLE_INSTRUMENTER_H_

#include "UnreachableInstrumenter.h"

class UnreachableInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of ExtractElementInstrumenter class.
     */
    UnreachableInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* UNREACHABLE_INSTRUMENTER_H_ */
