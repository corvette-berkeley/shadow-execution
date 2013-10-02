/**
 * @file UIToFPInstrumenter.h
 * @brief UIToFPInstrumenter Declarations
 */

#ifndef UI_TO_FP_INSTRUMENTER_H_
#define UI_TO_FP_INSTRUMENTER_H_

#include "UIToFPInstrumenter.h"

class UIToFPInstrumenter : public CastInstructionInstrumenter {
  public:
    UIToFPInstrumenter(std::string name, Instrumentation* instrumentation) :
      CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* TRUNC_INSTRUMENTER_H_ */
