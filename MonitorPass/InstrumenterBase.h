/**
 * @file InstrumenterBase.h
 * @brief InstrumenterBase Declarations.
 */

#ifndef INSTRUMENTER_BASE_H_
#define INSTRUMENTER_BASE_H_

#include "Common.h"
#include "Instrumentation.h"

class Instrumentation;

class InstrumenterBase {
public:
	InstrumenterBase(std::string name, Instrumentation* instrumentation)
	: name_(name), parent_(instrumentation), count_(0) {}

  /**
   * Deconstructor of InstrumenterBase class.
   */
	virtual ~InstrumenterBase(); 

  /**
   * Instrument the given instruction.
   *
   * @param I the LLVM instruction.
   * @return true if the instrumentation succeeds; false otherwise.
   */
	virtual bool CheckAndInstrument(Instruction* I); 

  /**
   * Report on the instrumentation progress.
   *
   * @return a report on the number of instructions successfully instrumented.
   */
	virtual std::string ReportToString(); 

protected:
	std::string name_; // name of the instrumenter.
	Instrumentation* parent_; // the parent instrumentation process.
	long count_; // counter of number of instructions successfully instrumented.

	friend class Instrumentation;
};

#endif /* INSTRUMENTER_BASE_H_ */
