/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef UREM_INSTRUMENTER_H_
#define UREM_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class URemInstrumenter : public BinaryOperatorInstrumenter {
public:
  URemInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
