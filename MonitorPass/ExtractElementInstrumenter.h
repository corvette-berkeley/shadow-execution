/**
 * @file ExtractElementInstrumenter.h
 * @brief
 */

#ifndef EXTRACT_ELEMENT_INSTRUMENTER_H_
#define EXTRACT_ELEMENT_INSTRUMENTER_H_

#include "UninterpretedInstrumenter.h"

class ExtractElementInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of ExtractElementInstrumenter class.
     */
    ExtractElementInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* EXTRACT_ELEMENT_INSTRUMENTER_H_ */
