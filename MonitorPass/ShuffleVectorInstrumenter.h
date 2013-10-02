/**
 * @file ShuffleVectorInstrumenter.h
 * @brief
 */

#ifndef SHUFFLE_VECTOR_INSTRUMENTER_H_
#define SHUFFLE_VECTOR_INSTRUMENTER_H_

#include "ShuffleVectorInstrumenter.h"

class ShuffleVectorInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of ExtractElementInstrumenter class.
     */
    ShuffleVectorInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* SHUFFLE_VECTOR_INSTRUMENTER_H_ */
