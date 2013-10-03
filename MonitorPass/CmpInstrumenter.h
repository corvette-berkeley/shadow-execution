/**
 * @file CmpInstrumenter.h
 * @brief CmpInstrumenter Declarations.
 */

#ifndef CMP_INSTRUMENTER_H_
#define CMP_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class CmpInstrumenter : public Instrumenter {
  public:
    CmpInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* CMP_INSTRUMENTER_H_ */
