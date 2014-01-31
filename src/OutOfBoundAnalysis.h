/**
 * @file OutOfBoundAnalysis.h
 * @brief OutOfBoundAnalysis Declarations.
 */

#ifndef OUT_OF_BOUND_ANALYSIS_H_
#define OUT_OF_BOUND_ANALYSIS_H_

#include "InterpreterObserver.h"
#include "IValue.h"

using namespace std;
using namespace llvm;

class OutOfBoundAnalysis : public InterpreterObserver {

  public:
    OutOfBoundAnalysis(std::string name) : InterpreterObserver(name) {}

    virtual void load(IID iid, KIND kind, KVALUE* op, int line, int inx);

    virtual void store(IID iid, KVALUE* dest, KVALUE* src, int file, int line, int inx);

    virtual void getelementptr(IID iid, bool inbound, KVALUE* op, KVALUE*
        index, KIND kind, uint64_t size, int line, int inx);

    virtual void getelementptr_array(IID iid, bool inbound, KVALUE* op, KIND
        kind, int elementSize, int inx);

    virtual void getelementptr_struct(IID iid, bool inbound, KVALUE* op, KIND
        kind, KIND arrayKind, int inx);

  private:
    bool isOutOfBound(IValue* pointer);
};

#endif /* OUT_OF_BOUND_ANALYSIS_H_ */
