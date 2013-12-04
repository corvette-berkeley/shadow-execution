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
  # reading command
  for line in executables:
    command = []
    for token in line.split():
      if (len(command) == 0):
        token = "./" + token + ".out" 
      command.append(token)
    print command

    # running command
    retval = call(command, stdin=None, stdout=None, stderr=None)

    # return -1 if running LLVM passes fails
    if retval <> 0:
      log.write("[FAILED RUNNING COMMAND]: " + command[0] + " with error code: " + str(retval) + "\n")
    else:
      log.write("[SUCCESSFULLY RUNNING COMMAND]: " + command[0] + " with error code: " + str(retval) + "\n")

if __name__ == "__main__":
  main()
