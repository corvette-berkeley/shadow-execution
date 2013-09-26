/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef SDIV_INSTRUMENTER_H_
#define SDIV_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class SDivInstrumenter : public BinaryOperatorInstrumenter {
public:
  SDivInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
