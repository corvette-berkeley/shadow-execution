/**
 * @file FCmpInstrumenter.h
 * @brief FCmpInstrumenter Declarations.
 */

#ifndef F_CMP_INSTRUMENTER_H_
#define F_CMP_INSTRUMENTER_H_

#include "CmpInstrumenter.h"

class FCmpInstrumenter : public CmpInstrumenter {
  public:
    FCmpInstrumenter(std::string name, Instrumentation* instrumentation) :
      CmpInstrumenter(name, instrumentation) {};
};

#endif /* I_CMP_INSTRUMENTER_H_ */
