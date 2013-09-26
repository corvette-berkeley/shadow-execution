/**
 * @file AndInstrumenter.h
 * @brief AndInstrumenter Declarations.
 */

#ifndef AND_INSTRUMENTER_H_
#define AND_INSTRUMENTER_H_

#include "BinaryOperatorInstrumenter.h"

class AndInstrumenter : public BinaryOperatorInstrumenter {
public:
  AndInstrumenter(std::string name, Instrumentation* instrumentation) :
    BinaryOperatorInstrumenter(name, instrumentation) {};
};

#endif
