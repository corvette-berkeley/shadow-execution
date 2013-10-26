/**
 * @file PHINodeInstrumenter.h
 * @brief
 */

#ifndef PHI_NODE_INSTRUMENTER_H_
#define PHI_NODE_INSTRUMENTER_H_

#include "Instrumenter.h"
#include "Common.h"

class PHINodeInstrumenter : public Instrumenter {
  public:
    /**
     * Constructor of PHINodeInstrumenter class.
     */
    PHINodeInstrumenter(std::string name, Instrumentation*
        instrumentation) : Instrumenter(name, instrumentation) {}

    bool CheckAndInstrument(Instruction* inst);
};

#endif /* PHI_NODE_INSTRUMENTER_H_ */
