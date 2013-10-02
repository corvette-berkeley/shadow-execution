/**
 * @file InsertElementInstrumenter.h
 * @brief
 */

#ifndef INSERT_ELEMENT_INSTRUMENTER_H_
#define INSERT_ELEMENT_INSTRUMENTER_H_

#include "UninterpretedInstrumenter.h"

class InsertElementInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of InsertElementInstrumenter class.
     */
    InsertElementInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* INSERT_ELEMENT_INSTRUMENTER_H_ */
