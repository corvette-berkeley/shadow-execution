/**
 * @file ExtractValueInstrumenter.h
 * @brief
 */

#ifndef EXTRACT_VALUE_INSTRUMENTER_H_
#define EXTRACT_VALUE_INSTRUMENTER_H_ 

#include "ExtractValueInstrumenter.h"

class ExtractValueInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of ExtractElementInstrumenter class.
     */
    ExtractValueInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* EXTRACT_VALUE_INSTRUMENTER_H_ */
