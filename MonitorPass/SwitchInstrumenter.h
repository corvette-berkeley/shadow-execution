/**
 * @file SwitchInstrumenter.h
 * @brief SwitchInstrumenter Declarations.
 */

#ifndef SWITCH_INSTRUMENTER_H_
#define SWITCH_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class SwitchInstrumenter : public Instrumenter {
  public:
    SwitchInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* inst); 
};

#endif /* SWITCH_INSTRUMENTER_H_ */
