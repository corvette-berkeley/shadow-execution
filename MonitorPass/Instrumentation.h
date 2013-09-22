
#ifndef INSTRUMENTATION_H_
#define INSTRUMENTATION_H_

#include "Common.h"
#include <map>

/*******************************************************************************************/

class Instrumentation; // see below

class InstrumenterBase {
public:
	InstrumenterBase(std::string name, Instrumentation* instrumentation)
	: name_(name), parent_(instrumentation), count_(0) {}
	virtual ~InstrumenterBase() {}

	// returns true iff any instrumentation was done
	virtual bool CheckAndInstrument(Instruction* I) = 0;

	virtual std::string ReportToString() {
		std::stringstream s;
		s << "Total " << name_ << " Instrumented: " << count_ << "\n";
		return s.str();
	}
protected:
	std::string name_;
	Instrumentation* parent_;
	long count_;

	friend class Instrumentation;
};

/*******************************************************************************************/

class Instrumentation {
public:
	typedef std::vector<InstrumenterBase*> InstrumenterPtrList;

	~Instrumentation();

	bool Initialize(Module &M);

	bool Finalize(Module &M);

	void BeginBasicBlock(BasicBlock* BB, Function* F, Module* M);

  void BeginFunction(); 

	// returns true iff any instrumentation was done
	bool CheckAndInstrument(Instruction* I);

  // map an instruction's IID to an index for fast access 
  // increase the number of variables
  void createIndex(uint64_t iid); 

  int getIndex(Instruction* inst);

	inline static Instrumentation* GetInstance() {
		if(instance_ == NULL) {
			instance_ = new Instrumentation();
		}
		return instance_;
	}

	void RegisterInstrumenter(InstrumenterBase* instrumenter);

public:
	BasicBlock* BB_;
	Function* F_;
	Module* M_;
	unsigned AS_;
  int varCount;
  std::map<uint64_t, int> indices;

private:
	InstrumenterPtrList instrumenters_;

	// singleton!!!
	Instrumentation() : BB_(NULL), F_(NULL), M_(NULL), AS_(0U) {}
	static Instrumentation* instance_;
};

/*******************************************************************************************/

#endif /* INSTRUMENTATION_H_ */
