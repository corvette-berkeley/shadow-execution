/**
 * @file ShlInstrumenter.h
 * @brief ShlInstrumenter Declarations.
 */

#ifndef SHL_INSTRUMENTER_H_
#define SHL_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class ShlInstrumenter : public BinaryOperatorInstrumenter {
public:
  ShlInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
