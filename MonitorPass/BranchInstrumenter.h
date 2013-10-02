/** @file BranchInstrumenter.h 
 * @brief BranchInstrumenter Declarations.
 */

#ifndef BRANCH_INSTRUMENTER_H_ 
#define BRANCH_INSTRUMENTER_H_

#include "Common.h"
#include "Instrumenter.h"

class BranchInstrumenter : public Instrumenter { 
  public:
    BranchInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* BRANCH_INSTRUMENTER_H_ */
