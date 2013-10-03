// this class includes all instrumenters


#ifndef INSTRUMENTERS_H_
#define INSTRUMENTERS_H_

#include "Common.h"
#include "Instrumenter.h"
#include "BinaryOperatorInstrumenter.h"
#include <llvm/Analysis/Verifier.h>

/*******************************************************************************************/

#define INSTR_TO_CALLBACK(inst)		("llvm_" inst)

/*******************************************************************************************/

// Callback: void switch_()
class SwitchInstrumenter : public Instrumenter {
  public:
    SwitchInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

};


// ***** Other Operations ***** //

#endif // INSTRUMENTERS_H_

