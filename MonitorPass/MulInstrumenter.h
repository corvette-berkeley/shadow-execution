/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef MUL_INSTRUMENTER_H_
#define MUL_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class MulInstrumenter : public BinaryOperatorInstrumenter {
public:
  MulInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
