/**
 * @file OrInstrumenter.h
 * @brief OrInstrumenter Declarations.
 */

#ifndef OR_INSTRUMENTER_H_
#define OR_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class OrInstrumenter : public BinaryOperatorInstrumenter {
public:
  OrInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
