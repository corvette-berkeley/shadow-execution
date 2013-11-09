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
  fname = sys.argv[1]
  names = open(fname, 'r')

  for name in names:
      name = name.strip()
      print name

      ##########################################
      # removing files

      command = ['rm', '-f', name + '.bc']
      call(command, stdin=None, stdout=None, stderr=None)

      command = ['rm', '-f', name + '.s']
      call(command, stdin=None, stdout=None, stderr=None)

      command = ['rm', '-f', name + '.ll']
      call(command, stdin=None, stdout=None, stderr=None)

  command = ['rm', '-f', 'tmppass.s']
  call(command, stdin=None, stdout=None, stderr=None)

  command = ['rm', '-f', 'tmppass.ll']
  call(command, stdin=None, stdout=None, stderr=None)



if __name__ == "__main__":
  main()
