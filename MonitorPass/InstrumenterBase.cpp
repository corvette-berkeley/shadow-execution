/**
 * @file InstrumenterBase.cpp
 * @brief
 */

#include "InstrumenterBase.h"

InstrumenterBase::~InstrumenterBase() {
}

bool InstrumenterBase::CheckAndInstrument(Instruction* I __attribute__((__unused__))) {
  return 0;
}

std::string InstrumenterBase::ReportToString() {
  std::stringstream s;
  s << "Total " << name_ << " Instrumented: " << count_ << "\n";
  return s.str();
}
