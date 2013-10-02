/**
 * @file SelectInstrumenter.h
 * @brief
 */

#ifndef SELECT_INSTRUMENTER_H_
#define SELECT_INSTRUMENTER_H_

#include "SelectInstrumenter.h"

class SelectInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of PHINodeInstrumenter class.
     */
    SelectInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* SELECT_INSTRUMENTER_H_ */
