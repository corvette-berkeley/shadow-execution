/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef FADD_INSTRUMENTER_H_
#define FADD_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class FAddInstrumenter : public BinaryOperatorInstrumenter {
public:
  FAddInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
