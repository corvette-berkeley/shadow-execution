/**
 * @file FDivInstrumenter.h
 * @brief FDivInstrumenter Declarations.
 */

#ifndef FDIV_INSTRUMENTER_H_
#define FDIV_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class FDivInstrumenter : public BinaryOperatorInstrumenter {
public:
  FDivInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
