/**
 * @file UninterpretedInstrumenter.h
 * @brief UninterpretedInstrumenter Declarations.
 */

#ifndef UNINTERPRETED_INSTRUMENTER_H_
#define UNINTERPRETED_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

/**
 * This is a super class for instrumenters of instructions in C that we do not
 * need to interpret. An example is those instructions that are only available
 * in C++. 
 */
class UninterpretedInstrumenter : public Instrumenter {
  public:
    /**
     * Constructor of UninterpretedInstrumenter class.
     */
    UninterpretedInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {}

    virtual bool CheckAndInstrument(Instruction* inst);

  private:
    UNINTERPRETEDINST getUninterpretedInst(Instruction* inst);
 };

#endif /* TERMINATOR_INSRUMENTER_H_ */
