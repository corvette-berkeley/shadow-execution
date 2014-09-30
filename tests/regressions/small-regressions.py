#!/usr/bin/env python

import os
import proc
import platform
import sys
from subprocess import CalledProcessError, PIPE, Popen, call


#
# main function receives
#   - argv[1] : list of executables from which the bitcode is to be extracted
#
def main():
  executables_name = sys.argv[1]
  executables = open(executables_name, 'r')

  # setting some variables
  llvm = os.getenv("LLVM_BIN_PATH")
  monitorpath = os.getenv("MONITOR_LIB_PATH")

  if platform.system() == 'Darwin':
      monitorpass = monitorpath + "/MonitorPass.dylib"
  else:
      monitorpass = monitorpath + "/MonitorPass.so"

  sourcepath = os.getenv("INSTRUMENTOR_LIB_PATH");
  glogpath = os.getenv("GLOG_LIB_PATH")
  glog_log_dir = os.getenv("GLOG_log_dir")

  logfile = "log.out"
  log = open(logfile, "w")

  for executable in executables:
      executable = executable.strip()
      print executable

      ##########################################
      # compiling file
      bitcodefile = executable + '.bc'
      bitcode = open(bitcodefile, 'w')

      command = [llvm + '/clang', '-c', '-emit-llvm', executable + '.c', '-fpack-struct', '-o', bitcodefile]
      retval = call(command, stdin=None, stdout=None, stderr=None)

      # return -1 if running LLVM passes fails
      if retval <> 0:
        log.write("[FAILED INSTRUMENTATION]: i_" + executable + ".bc\n")
        continue

      # creating ll file for original program
      command = [llvm +'/llvm-dis', bitcodefile, '-o', executable + '-orig.ll']
      call(command, stdin=None, stdout=None, stderr=None)

      ##########################################
      # breaking getelementptr constants
      gbitcodefile = 'g_' + executable + '.bc'
      gbitcode = open(gbitcodefile, 'w')

      command = [llvm + '/opt', '-load', monitorpass, '--break-constgeps', bitcodefile, '-f', '-o', gbitcodefile]
      retval = call(command, stdin=None, stdout=None, stderr=None)

      # return -1 if running LLVM passes fails
      if retval <> 0:
        log.write("[FAILED BREAKING GETELEMENTPTR CONSTANTS]: g_" + executable + ".bc\n")
        continue

      ##########################################
      # instrumented bitcode file
      ibitcodefile = 'i_' + executable + '.bc'
      ibitcode = open(ibitcodefile, 'w')

      command = [llvm + '/opt', '-load', monitorpass, '--instrument', gbitcodefile, '--file', glog_log_dir + '/' + executable + '-metadata.txt', '-o', ibitcodefile]
      retval = call(command, stdin=None, stdout=None, stderr=None)

      # return -1 if running LLVM passes fails
      if retval <> 0:
        log.write("[FAILED INSTRUMENTATION]: i_" + executable + ".bc\n")
        continue

      # creating ll file for instrumented code
      command = [llvm + '/llvm-dis', ibitcodefile, '-o', executable + '.ll']
      call(command, stdin=None, stdout=None, stderr=None)

      ###########################################
      # instrumented executable file
      '''
      iassemblyfile = 'i_' + executable + '.s'
      iassembly = open(iassemblyfile, 'w')

      command = [llvm + '/llc', ibitcodefile]
      retval = call(command, stdin=None, stdout=None, stderr=None)

      # return -1 if running LLVM passes fails
      if retval <> 0:
        log.write("[FAILED ASSEMBLY]: " + executable + ".s\n")
        continue
      '''

      ############################################
      # creating instrumented executable file
      iexecutablefile = executable + '.out'
      iexecutable = open(iexecutablefile, 'w')

      # add -lgmp for expr and other core utility
      command = [llvm + '/clang', '-use-gold-plugin', ibitcodefile, '-L' + sourcepath, '-L' + glogpath, '-lmonitor', '-lpthread', '-lm', '-lglog', '-o', iexecutablefile]
      retval = call(command, stdin=None, stdout=iexecutable, stderr=None)

      # return -1 if running LLVM passes fails
      if retval <> 0:
        log.write("[FAILED CREATING EXECUTABLE]: " + executable + ".out\n")

      iexecutable.close()


      ############################################
      # running executable file
      command = ['valgrind', './' + executable + '.out']
      retval = call(command, stdin=None, stdout=None, stderr=None)

      # return -1 if running LLVM passes fails
      if retval <> 0:
        log.write("[FAILED RUNNING EXECUTABLE]: " + iexecutablefile + " with error code " + str(retval) + "\n")




if __name__ == "__main__":
  main()
