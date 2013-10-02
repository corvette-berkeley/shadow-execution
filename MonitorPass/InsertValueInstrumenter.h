/**
 * @file InsertValueInstrumenter.h
 * @brief
 */

#ifndef INSERT_VALUE_INSTRUMENTER_H_
#define INSERT_VALUE_INSTRUMENTER_H_

#include "InsertValueInstrumenter.h"

class InsertValueInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of ExtractElementInstrumenter class.
     */
    InsertValueInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* INSERT_VALUE_INSTRUMENTER_H_ */
