/**
 * @file AddInstrumenter.h
 * @brief AddInstrumenter Declarations.
 */

#ifndef SUB_INSTRUMENTER_H_
#define SUB_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class SubInstrumenter : public BinaryOperatorInstrumenter {
public:
  SubInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
