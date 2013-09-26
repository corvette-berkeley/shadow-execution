/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef ADD_INSTRUMENTER_H_
#define ADD_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class AddInstrumenter : public BinaryOperatorInstrumenter {
public:
  AddInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
