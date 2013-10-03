/**
 * @file ReturnInstrumenter.h
 * @brief
 */

#ifndef RETURN_INSTRUMENTER_H_
#define RETURN_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class ReturnInstrumenter : public Instrumenter {
  public:
    ReturnInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* RETURN_INSTRUMENTER_H_ */
