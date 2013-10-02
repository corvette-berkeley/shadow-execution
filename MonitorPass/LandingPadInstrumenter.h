/**
 * @file LandingPadInstrumenter.h
 * @brief
 */

#ifndef LANDING_PAD_INSTRUMENTER_H_
#define LANDING_PAD_INSTRUMENTER_H_

#include "LandingPadInstrumenter.h"

class LandingPadInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of LandingPadInstrumenter class.
     */
    LandingPadInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* LANDING_PAD_INSTRUMENTER_H_ */
