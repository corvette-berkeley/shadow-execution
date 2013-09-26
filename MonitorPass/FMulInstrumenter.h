/**
 * @file FMulInstrumenter.h
 * @brief FMulInstrumenter Declarations.
 */

#ifndef FMUL_INSTRUMENTER_H_
#define FMUL_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class FMulInstrumenter : public BinaryOperatorInstrumenter {
public:
  FMulInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
