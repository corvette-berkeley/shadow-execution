/**
 * @file CastInstructionInstrumenter.h
 * @brief CastInstructionInstrumenter Declarations.
 */

#ifndef CAST_INSTRUCTION_INSTRUMENTER_H_
#define CAST_INSTRUCTION_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class CastInstructionInstrumenter : public Instrumenter {
  public:
    /**
     * Constructor of CastInstructionInstrumenter class.
     */
    CastInstructionInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {}

    virtual bool CheckAndInstrument(Instruction* inst);

  private:
    /**
     * Get CASTOP value for the given instruction.
     *
     * @note CASTOP is defined in Constants.h.
     *
     * @param inst the LLVM instruction.
     *
     * @return the CASTOP value associated with the given instruction.
     */
    CASTOP getCastOp(CastInst* inst);
};

#endif // CAST_INSTRUCTION_INSTRUMENTER_H_
