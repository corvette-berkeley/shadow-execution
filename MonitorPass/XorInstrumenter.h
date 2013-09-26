/**
 * @file XorInstrumenter.h
 * @brief XorInstrumenter Declarations.
 */

#ifndef XOR_INSTRUMENTER_H_
#define XOR_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class XorInstrumenter : public BinaryOperatorInstrumenter {
public:
  XorInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
