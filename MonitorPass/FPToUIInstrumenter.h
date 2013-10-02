/**
 * @file FPToUIInstrumenter.h
 * @brief FPToUIInstrumenter Declarations
 */

#ifndef FP_TO_UI_INSTRUMENTER_H_
#define FP_TO_UI_INSTRUMENTER_H_

#include "CastInstructionInstrumenter.h"

class FPToUIInstrumenter : public CastInstructionInstrumenter {
  public:
    FPToUIInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* FP_TO_UI_INSTRUMENTER_H_ */
