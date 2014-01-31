#!/usr/bin/env python

import os
import proc
import platform
import sys
from subprocess import CalledProcessError, PIPE, Popen, call


#
# main function receives
#   - argv[1] : list of commands to execute
#
def main():
  executables_name = sys.argv[1]
  executables = open(executables_name, 'r')

  logfile = "log-run.out"
  log = open(logfile, "w") 

  ############################################
  # running uninstrumented code
  
  # reading command
  for line in executables:
    command = []
    for token in line.split():
      if (len(command) == 0):
        token = "bin/" + token 
      command.append(token)
    print command

    # revert list.txt
    revert_command = ["cp", "all-utilities.txt", "list.txt"]
    call(revert_command, stdin=None, stdout=None, stderr=None)

    # running command
    retval_uninstrumented = call(command, stdin=None, stdout=None, stderr=None)

  ############################################
  # running instrumented code
    
  # reading command
    command = []
    for token in line.split():
      if (len(command) == 0):
        token = "./" + token + ".out" 
      command.append(token)
    print command

    # revert list.txt
    revert_command = ["cp", "all-utilities.txt", "list.txt"]
    call(revert_command, stdin=None, stdout=None, stderr=None)

    # running command
    retval_instrumented = call(command, stdin=None, stdout=None, stderr=None)

    # TODO: check for errno or other values
    if retval_uninstrumented <> retval_instrumented:
      log.write("[FAILED ERROR CODE MISMATCH]: " + command[0] + " with error codes: " + str(retval_uninstrumented) + " " + str(retval_instrumented) + "\n")
    elif retval_instrumented <> 0:
      log.write("[FAILED RUNNING COMMAND]: " + command[0] + " with error code: " + str(retval_instrumented) + "\n")
    else:
      log.write("[SUCCESSFULLY RUNNING COMMAND]: " + command[0] + " with error code: " + str(retval_instrumented) + "\n")

if __name__ == "__main__":
  main()
