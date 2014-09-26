#!/usr/bin/env python2

# fs-copy dir

import jobmonitor
import subprocess
import sys

FUDGE_PERCENT=0.3

executable = "./" + sys.argv[1]+".out"

result = jobmonitor.RunJob([executable],shell = False, stdout=None, stderr=None)

names = ["Command", "Return Code", "Runtime", "Max memory usage"]

#f = open(executable+".result.ref")
f = ['Foobar\n', "0\n", "300\n", "10000000\n"]
for new_val, str_ref_val, name in zip(result, f, names):
	str_ref_val = str_ref_val[0:-1]
	if name == names[0] and new_val != str_ref_val:
		print("Command ran was different - error!: Expected "+str_ref_val + " but got " + str(new_val))
		#exit(1)

	if name == names[0]:
		continue
	ref_val = float(str_ref_val)
	if new_val > ref_val * (1+FUDGE_PERCENT):
		print("New value for " + name + " is much worse than old value: " + str(new_val) + " > " + str(ref_val) + " * " + str(1+FUDGE_PERCENT))
		#exit(1)
print(result[0]+" returned " + str(result[1]) + ", using " + str(result[2]) + " seconds and " + str(result[3]) + " bytes")
