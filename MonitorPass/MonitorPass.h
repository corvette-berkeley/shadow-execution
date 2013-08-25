#ifndef LLVM_TRANSFORMS_MONITOR_PASS_H_
#define LLVM_TRANSFORMS_MONITOR_PASS_H_

namespace llvm {

  class FunctionPass;

  FunctionPass* createMonitorPass();

} // end namespace llvm


#endif // LLVM_TRANSFORMS_MONITOR_PASS_H_
