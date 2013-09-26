/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef UDIV_INSTRUMENTER_H_
#define UDIV_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class UDivInstrumenter : public BinaryOperatorInstrumenter {
public:
  UDivInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
