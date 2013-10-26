/**
 * @file ExtractValueInstrumenter.h
 * @brief
 */

#ifndef EXTRACT_VALUE_INSTRUMENTER_H_
#define EXTRACT_VALUE_INSTRUMENTER_H_ 

#include "Instrumenter.h"

class ExtractValueInstrumenter : public Instrumenter {
  public:
    /**
     * Constructor of ExtractElementInstrumenter class.
     */
    ExtractValueInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {}

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* EXTRACT_VALUE_INSTRUMENTER_H_ */
