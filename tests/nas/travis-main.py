#!/usr/bin/env python2

# fs-copy dir
from __future__ import print_function

import jobmonitor
import subprocess
import sys

def main():
	executable = "./" + sys.argv[1]+".out"

	result = jobmonitor.RunJob([executable],shell = False, stdout=None, stderr=None)

	check_commands = {"Command" : checkCommand, "Return Code" : checkRC, "Runtime" : checkRuntime, "Memory" : checkMemory }

	names = ["Command", "Return Code", "Runtime", "Memory"]

	fnew = open(executable+".result.new", 'w')
	for a in result:
		print(str(a), file=fnew)
	fnew.close()
	f = open(executable+".result.ref", 'r')

	#f = [ str(a)+"\n" for a in result]
	print(str(result[0])+" returned " + str(result[1]) + ", using " + str(result[2]) + " seconds and " + str(result[3]) + " bytes")

	for new_val, str_ref_val, name in zip(result, f, names):
		if not check_commands[name](str(new_val), str_ref_val[0:-1]):
			exit(1)

def checkCommand(new_val, old_val):
	same = new_val == old_val
	if not same:
		print("Command ran was different - error!: Expected "+str_ref_val + " but got " + str(new_val))
	return same

def checkRC(new_val, old_val):
	same = int(new_val) == int(old_val)
	if not same:
		print("Return Code has changed! Expected " + old_val + ", but got " + new_val);
	if same and int(old_val) < 0: # exit rcs are the same, but both errored
		print("Since the program errored, we are skipping memory and runtime checks!")
		exit(0)
	return same

def compareFloats(new_val, old_val, name):
	FUDGE_PERCENT=0.35
	same = float(new_val) < float(old_val)*(1+FUDGE_PERCENT)
	if not same:
		print(name + " has changed! Expected value to be at most " + old_val + " * " + str(1+FUDGE_PERCENT) + ", but is " + new_val);
		return same
	if same and float(new_val)*(1+FUDGE_PERCENT*2) < float(old_val):
		print("WARNING: " + name + " has improved significantly! Expected reference to be at least " + new_val + " * " + str(1+FUDGE_PERCENT*3) + ", but reference is " + old_val);
	return same

def checkRuntime(a,b):
	return compareFloats(a,b, "Runtime")

def checkMemory(a,b):
	return compareFloats(a,b, "Max memory")

main()
