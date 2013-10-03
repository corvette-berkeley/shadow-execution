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

// ***** TerminatorInst ***** //

// Callback: void indirectbr()
class IndirectBrInstrumenter : public Instrumenter {
  public:
    IndirectBrInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(IndirectBrInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);
      Value* op1 = KVALUE_VALUE(SI->getAddress(), Instrs, NOSIGN);
      if (op1 == NULL) return false;
      Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("indirectbr"), C_iid, op1, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// Callback: void switch_()
class SwitchInstrumenter : public Instrumenter {
  public:
    SwitchInstrumenter(std::string name, Instrumentation* instrumentation) :
      Instrumenter(name, instrumentation) {};

    bool CheckAndInstrument(Instruction* I) {
      CAST_OR_RETURN(SwitchInst, SI, I);

      safe_assert(parent_ != NULL);

      count_++;

      InstrPtrVector Instrs;

      Constant* C_iid = IID_CONSTANT(SI);

      Value* op = SI->getCondition();

      Instruction* call = CALL_IID_KVALUE_INT(INSTR_TO_CALLBACK("switch_"), C_iid, op, computeIndex(SI));
      Instrs.push_back(call);

      // instrument
      InsertAllBefore(Instrs, SI);

      return true;
    }
};


// ***** Other Operations ***** //

#endif // INSTRUMENTERS_H_

