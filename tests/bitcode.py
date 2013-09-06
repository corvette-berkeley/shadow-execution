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
  for executable in executables:
      executable = executable.strip()
      print executable
      command = ['/scratch/whole-program-llvm/extract-bc', executable]
      call(command, stdin=None, stdout=None, stderr=None)

if __name__ == "__main__":
  main()
