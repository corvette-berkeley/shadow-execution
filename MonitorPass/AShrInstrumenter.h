/**
 * @file AShlInstrumenter.h
 * @brief AShlInstrumenter Declarations.
 */

#ifndef ASHR_INSTRUMENTER_H_
#define ASHR_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class AShrInstrumenter : public BinaryOperatorInstrumenter {
public:
  AShrInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
