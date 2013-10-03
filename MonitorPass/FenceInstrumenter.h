/**
 * @file FenceInstrumenter.h
 * @brief FenceInstrumenter Declarations.
 */

#ifndef FENCE_INSTRUMENTER_H_
#define FENCE_INSTRUMENTER_H_

#include "UninterpretedInstrumenter.h"

class FenceInstrumenter : public UninterpretedInstrumenter {
  public:
    FenceInstrumenter(std::string name, Instrumentation* instrumentation) :
      UninterpretedInstrumenter(name, instrumentation) {};
};

#endif
