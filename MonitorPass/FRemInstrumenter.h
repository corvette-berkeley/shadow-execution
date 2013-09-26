/**
 * @file FRemInstrumenter.h
 * @brief FRemInstrumenter Declarations.
 */

#ifndef FREM_INSTRUMENTER_H_
#define FREM_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class FRemInstrumenter : public BinaryOperatorInstrumenter {
public:
  FRemInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
