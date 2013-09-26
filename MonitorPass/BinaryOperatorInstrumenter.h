/**
 * @file BinaryOperatorInstrumenter.h
 * @brief BinaryOperatorInstrumenter Declarations.
 */

#ifndef BINARY_OPERATORS_INSTRUMENTER_H_
#define BINARY_OPERATORS_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class BinaryOperatorInstrumenter : public Instrumenter {
  public:
    /**
     * Constructor of BinaryOperatorInstrumenter class.
     */
    BinaryOperatorInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {}

    virtual bool CheckAndInstrument(Instruction* inst);
  private:
    /**
     * Get BINOP value for the given instruction.
     *
     * @note BINOP is defined in Constants.h.
     *
     * @param inst the LLVM instruction.
     *
     * @return the BINOP value associated with the given instruction.
     */
    BINOP getBinOp(BinaryOperator* inst); 
};

#endif /* BINARY_OPERATORS_INSTRUMENTER_H_ */
