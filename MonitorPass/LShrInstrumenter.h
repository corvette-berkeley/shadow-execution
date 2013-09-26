/**
 * @file LShlInstrumenter.h
 * @brief LShlInstrumenter Declarations.
 */

#ifndef LSHR_INSTRUMENTER_H_
#define LSHR_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class LShrInstrumenter : public BinaryOperatorInstrumenter {
public:
  LShrInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
