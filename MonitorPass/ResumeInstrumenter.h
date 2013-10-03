/**
 * @file ResumeInstrumenter.h
 * @brief ResumeInstrumenter Declarations.
 */

#ifndef RESUME_INSTRUMENTER_H_
#define RESUME_INSTRUMENTER_H_

#include "UninterpretedInstrumenter.h"

class ResumeInstrumenter : public UninterpretedInstrumenter {
  public:
    ResumeInstrumenter(std::string name, Instrumentation* instrumentation) :
      UninterpretedInstrumenter(name, instrumentation) {};
};

#endif /* RESUME_INSTRUMENTER_H_ */
