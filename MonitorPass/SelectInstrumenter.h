/**
 * @file SelectInstrumenter.h
 * @brief
 */

#ifndef SELECT_INSTRUMENTER_H_
#define SELECT_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class SelectInstrumenter : public Instrumenter {
  public:
    /**
     * Constructor of SelectInstrumenter class.
     */
    SelectInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {}

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* SELECT_INSTRUMENTER_H_ */
