/**
 * @file ICmpInstrumenter.h
 * @brief ICmpInstrumenter Declarations.
 */

#ifndef I_CMP_INSTRUMENTER_H_
#define I_CMP_INSTRUMENTER_H_

#include "CmpInstrumenter.h"

class ICmpInstrumenter : public CmpInstrumenter {
  public:
    ICmpInstrumenter(std::string name, Instrumentation* instrumentation) :
      CmpInstrumenter(name, instrumentation) {};
};

#endif /* I_CMP_INSTRUMENTER_H_ */
