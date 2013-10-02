/**
 * @file FPTruncInstrumenter.h
 * @brief FPTruncInstrumenter Declarations
 */

#ifndef FP_TRUNC_INSTRUMENTER_H_
#define FP_TRUNC_INSTRUMENTER_H_

#include "CastInstructionInstrumenter.h"

class FPTruncInstrumenter : public CastInstructionInstrumenter {
  public: 
    FPTruncInstrumenter(std::string name, Instrumentation* instrumentation) :
    CastInstructionInstrumenter(name, instrumentation) {};
};

#endif /* FP_TRUNC_INSTRUMENTER_H_ */
