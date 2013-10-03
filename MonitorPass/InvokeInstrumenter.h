/**
 * @file InvokeInstrumenter.h
 * @brief InvokeInstrumenter Declarations.
 */

#ifndef INVOKE_INSTRUMENTER_H_
#define INVOKE_INSTRUMENTER_H_

#include "UninterpretedInstrumenter.h"

class InvokeInstrumenter : public UninterpretedInstrumenter {
  public:
    InvokeInstrumenter(std::string name, Instrumentation* instrumentation) :
      UninterpretedInstrumenter(name, instrumentation) {};
};

#endif /* INVOKE_INSTRUMENTER_H_ */
