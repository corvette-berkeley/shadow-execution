/**
 * @file PHINodeInstrumenter.h
 * @brief
 */

#ifndef PHI_NODE_INSTRUMENTER_H_
#define PHI_NODE_INSTRUMENTER_H_

#include "PHINodeInstrumenter.h"

class PHINodeInstrumenter : public UninterpretedInstrumenter {
  public:
    /**
     * Constructor of PHINodeInstrumenter class.
     */
    PHINodeInstrumenter(std::string name, Instrumentation*
        instrumentation) : UninterpretedInstrumenter(name, instrumentation) {}
};

#endif /* PHI_NODE_INSTRUMENTER_H_ */
