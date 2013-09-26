/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef SREM_INSTRUMENTER_H_
#define SREM_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class SRemInstrumenter : public BinaryOperatorInstrumenter {
public:
  SRemInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
