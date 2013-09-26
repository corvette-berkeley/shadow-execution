/**
 * @file FSubInstrumenter.h
 * @brief FSubInstrumenter Declarations.
 */

#ifndef FSUB_INSTRUMENTER_H_
#define FSUB_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class FSubInstrumenter : public BinaryOperatorInstrumenter {
public:
  FSubInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
